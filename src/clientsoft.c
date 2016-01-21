#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LNG 18
// キーボード入力文字列の最大長
#define LEN2 32
// ヌル文字を含んだコマンド (引数含まず) の最大長
#define CMDLEN 8
// コマンドの数
#define NUMCMD 32

// プロトタイプ宣言
// void inputcmd(char*);
// int string2argv(char*, char**);
// void cmdlist(void);

int main(int argc, char** argv)
{
	// i, j, k はループ変数、numcmd はコマンドの数、cmdargc はプロンプト上での引数の数
	int sd, len, i, j, k, numcmd, cmdargc;
	int cmdlen; // 送信コマンドの長さ
	char a = 0, g = 0, s = 0, c = 0, l = 0; // フラグ用、0で未入力、1で入力済み
	char call1[8], call2[8], header[32]={}, buff[LEN2]={}, netaddr[64], cmdpath[256], logpath[256];
	char cmdlist[NUMCMD][CMDLEN], cmdargv[8][8];
	int cmd;		// 実行するコマンドの番号
	int arglen;
	char txarg[32];		// 送信引数を格納
	char d[LNG] = {};
	char numtx=0; // 送信コマンドの通し番号
	struct sockaddr_in addr;
	FILE *fpcmd, *fplog;
	
	// 一時的に書いている
	strcpy(cmdlist[0], "DND");
	strcpy(cmdlist[1], "DTD");
	strcpy(cmdlist[2], "DRD");
	strcpy(cmdlist[3], "DHD");
	strcpy(cmdlist[4], "DDD");
	strcpy(cmdlist[5], "DMD");
	strcpy(cmdlist[6], "DLD");
	strcpy(cmdlist[7], "SUC");
	strcpy(cmdlist[8], "DFC");
	strcpy(cmdlist[9], "DRR");
	strcpy(cmdlist[10], "DIR");
	strcpy(cmdlist[11], "KsC");
	strcpy(cmdlist[12], "KSc");
	strcpy(cmdlist[13], "RST");
	strcpy(cmdlist[14], "RMM");
	strcpy(cmdlist[15], "RCM");
	strcpy(cmdlist[16], "DBS");
	strcpy(cmdlist[17], "EAA");
	strcpy(cmdlist[18], "DPD");
	strcpy(cmdlist[19], "DSD");
	strcpy(cmdlist[20], "CMD");
	numcmd = 21;
	
	// コマンドライン上の引数の処理
	i = 1;
	while(i<argc){
		if((argv[i][0] == '-') && (argv[i][2] == 0)){ // ハイフンで始まる2文字の引数かどうか
			switch(argv[i][1]){ // 引数2文字目の値によって分岐
			  case 'A':
			  case 'a':
				// スペースに続く次の文字列は TNC のアドレス
				i++;
				strcpy(netaddr, argv[i]);
				a = 1; // 入力済みフラグ。以下同様
				break;
				
			  case 'G':
			  case 'g':
				// スペースに続く次の文字列は地上局コールサイン
				i++;
				strcpy(call1, argv[i]);
				g = 1;
				break;
				
			  case 'S':
			  case 's':
				// スペースに続く次の文字列は人工衛星局コールサイン
				i++;
				strcpy(call2, argv[i]);
				s = 1;
				break;
				
			  case 'C':
			  case 'c':
				// スペースに続く次の文字列はコマンド一覧のファイルへのパス
				i++;
				strcpy(cmdpath, argv[i]);
				c = 1;
				break;
				
			  case 'L':
			  case 'l':
				// スペースに続く次の文字列はログファイルへのパス
				i++;
				strcpy(logpath, argv[i]);
				l = 1;
				break;
				
			  default:
				break;
			}
		}else{
			/* エラーか使用法 */
			printf("unknown option\n");
		}
		i++;
	}
	
	
	// フラグで指定されなかった項目の入力
	if(g != 1){
		printf("\x1b[32mInput Ground Station call sign: ");
		scanf("%s", call1);
	}
	if(s != 1){
		printf("Input Satellite Station call sign: ");
		scanf("%s", call2);
	}
	if(a != 1){
		printf("Input TNC Address: ");
		scanf("%s", netaddr);
	}
	
	printf("Ground Call Sign: %s\n", call1);
	printf("Satellite Call Sign: %s\n", call2);
	printf("TNC Address: %s\n", netaddr);
	// printf("Command List: %s\n", cmdpath);
	// printf("Log File: %s\n", logpath);
	
    // IPv4 TCP のソケットを作成する
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // 送信先アドレスとポート番号を設定する
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	addr.sin_addr.s_addr = inet_addr(netaddr);
	
    // サーバ接続（TCP の場合は、接続を確立する必要がある）
	connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

// ヘッダ作成 (ただし冒頭の 0xc0, 0x00 を含まない)
    strcat(header, call1);
    strcat(header, " >");
    strcat(header, call2);
    strcat(header, " :");

	// コマンド一覧を表示
	printf("List of commands\n");
	for(i=0;i<numcmd;i++){
		printf("%d:\t%s\t\t", i, cmdlist[i]);
		if(i%3 == 2){
			printf("\n");
		}
	}
	printf("\n");

	
// プロンプト画面
    while(1){
	printf("%s", header);
	fgets(buff, LEN2, stdin);
	
	// 入力文字列 (buff) をスペース区切りで二次元配列 (cmdargv) に格納。
	// スペース連続時の対応→後で
	i = 0; j = 0; k = 0;
	while((buff[k] != 0) && (buff[k] != '\n')){
		if(buff[k] == 0x20){
			cmdargv[i][j] = 0; // ヌル文字の代入
			i++;
			j = 0;
		}else{
			cmdargv[i][j] = buff[k];
			j++;
		}
		k++;
	}
	cmdargv[i][j] = 0; // 最後に代入した文字の次にヌル文字を置く。
	cmdargc = i + 1; // プロンプトで入力した引数の数プラス1
	
	/*
	// cmdargv の内容を表示、デバッグ用
	printf("cmdargc = %d\n", cmdargc);
	for(i=0;i<cmdargc;i++){
		printf("%d: %s\n", i, cmdargv[i]);
	}
	*/
	
	
	// 格納された二次元配列を処理する -> あとで関数にしたいね
	
	// 一番最初の文字列だけ処理
	cmd = atoi(cmdargv[0]); // 一番最初の文字列はコマンドの番号とする。暫定的
	
	// 2つめ以降 (すなわち引数) の文字列処理
	i = 1; arglen = 0;
	while(i<cmdargc){
		// 以下、if 文内では各二次元配列の先頭の文字で条件分岐
		if((0x30 <= cmdargv[i][0]) && (cmdargv[i][0] <= 0x39)){
			// 文字列を数字に変換
			txarg[arglen] = atoi(cmdargv[i]);
			arglen++;
		}else if(cmdargv[i][0] == '-'){
			switch(cmdargv[i][1]){
			  case 't':
			  	i++;
			  	//
			  	break;
			  default:
			  	printf("unknown option\n");
				break;
			}
		}
		i++;
	}
	
	/*
	// txarg の内容を10進数で表示。デバッグ用
	printf("txarg: ");
	for(i=0;i<cmdargc-1;i++){
		printf("%d ", txarg[i]);
	}
	printf("\n");
	printf("arglen: %d\n", arglen);
	*/

	// 数字をコマンドに変換する関数とか、コマンド一覧をファイルから配列に読み込む関数を作っておくと便利かも
	// 明らかにテキストの部分だけ先に文字列関数で処理しておいてバイナリを後で置き換える。
	// 置き換える部分を把握するため、必要に応じて strlen で長さを取得。
	
	// コマンド文字列を作る
	
	cmd = atoi(cmdargv[0]); // プロンプトの最初の引数 = コマンド番号
	
	// cmd2str(buff, cmd);	
	// ヘッダーとコマンド文字列の連結
	strcat(d, "  ");
	strcat(d, header);
	strcat(d, " ");			// 通し番号のバイナリ部分にダミーのスペース
	strcat(d, cmdlist[cmd]);
	len = strlen(d);		// ヘッダー、通し番号、コマンドまでの長さ
	
	// printf("d: %s\n", d);	// デバッグ用
	
	for(i=0;i<arglen;i++){
		d[len+i] = txarg[i];
	}
	
	d[0] = 0xC0; d[1] = 0x00;	// ヘッダの1～2バイト目に正しいバイナリを入れる
	i = strlen(header);
	d[i+2] = numtx;			// コマンドの直前に正しい通し番号を入れる。
	d[len+arglen] = 0xC0; d[len+arglen+1] = 0;	// フッター
	send(sd, d, len+arglen+2, 0);		// d の文字列 len+arglen+2 バイト分を喪神 ！！
	d[0] = 0;			// initialize d
	
	numtx++;
    }

    // exit kiss connection
//    int exitkisscommand[3] = {0xC0, 0xFF, 0xC0};
//    send(sd, &exitkisscommand, 3, 0);

    close(sd);

    return 0;
}



// キーボードからの入力を促し、数字やフラグを適切な文字列に変換。
// help, quit 等コマンドも受け付けて処理。

void num2cmd(int num, char *cmd[])
{
	return;
}



// 起動時のフラグで指定するもの
// 接続先アドレス a、地上局コールサイン g、人工衛星コールサイン s、コマンド一覧ファイル c、ログファイル l

// 起動後のプロンプトで受け付けるもの
// コマンド番号、テキスト・バイナリのスイッチ -t, -s、help, quit 等のコマンド

// 数字をコマンドに変換する関数に必要な引数: 数字と元の
// main 関数でファイル名を入力、ファイルをオープン、main関数で宣言した配列に格納

// 問題点: もしかするとヌル文字が入っていると送信が途中で切れるかも
