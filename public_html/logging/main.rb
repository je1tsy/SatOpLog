#!/usr/local/bin/ruby
# -*- coding: utf-8 -*-
require 'cgi'
cgi = CGI.new

print cgi.header("text/html; charset = utf-8")

#print header with javascript function and body
print <<EOF
<head>
<h2>
  SatelliteOperation for ITF-2
</h2>
<p>University of Tsukuba / Maintainer: JE6HBC</p>
<meta http-equiv="Content-Style-Type" content="text/css" />
<link rel="stylesheet" type="text/css" href="../logging_css/satlog.css">
<script language="Javascript">
<!--
function inserttc( textid, ischecked ){
  if( ischecked == true) {
    document.getElementById(textid).disabled = false;
    document.getElementById(textid).value = myJST;
  }
  else{
    document.getElementById(textid).disabled = true;
  }
}
function inserttcsubmit( textid, ischecked ){
  if( ischecked == true) {
    document.getElementById(textid).disabled = false;
    log = myJST;
    log += ', ';
    log += textid;
    document.getElementById(textid).value = log;
  }
  else{
    document.getElementById(textid).disabled = false;
  }
}
</script>
</head>
<body>
<div id="TimeCode">
  <script language="Javascript">
  <!--
  function myTimeprev(){
    var Nowymdhms = new Date();
    var NowYear = Nowymdhms.getYear() + 1900;
    var NowMon = Nowymdhms.getMonth() + 1;
    var NowDay = Nowymdhms.getDate();
    var NowWeek = Nowymdhms.getDay();
    var NowHour = Nowymdhms.getHours();
    var NowMin = Nowymdhms.getMinutes();
    var NowSec = Nowymdhms.getSeconds();
    var Week = new Array("Sunday","Monday","Tueday","Wednesday","Thursday","Friday","Saturday");
    myJST = NowYear + "/" + NowMon + "/" + NowDay + "("+Week[NowWeek]+") "+ NowHour + ":" + NowMin + ":" + NowSec;
    var UNowHour = NowHour - 9;
    myUST = NowYear + "/" + NowMon + "/" + NowDay + "("+Week[NowWeek]+") "+ UNowHour + ":" + NowMin + ":" + NowSec;
    document.myTime.myJST.value = myJST ;
    document.myTime.myUST.value = myUST
  }
  // -->
  </script>
  <script language="JavaScript">
  <!--
    setInterval("myTimeprev()", 1000);
  // -->
  </script>
  <form name="myTime">
  <input size="2" value="JST">
  <input type="text" size="25" name="myJST">
  <br>
  <input size="2" value="UST">
  <input type="text" size="25" name="myUST">
  </form>
</div>
EOF
print <<EOF
  <form action="vote.rb" method="post">
EOF

#Read contents from question.txt and show checklist
io = open("action_list.txt", "r:UTF-8")
title = io.gets
  print <<EOF
    <p class="Checklist">
      <h2>#{title}</h2>
EOF
while line = io.gets
  sline = line.chomp
  print <<EOF
<div><INPUT type="text" id="tcOf#{sline}" size="30" disabled="true" value=""><INPUT type="checkbox" name="selects" id="tcSubmit#{sline}" onclick="inserttc('tcOf#{sline}', this.checked);inserttcsubmit('tcSubmit#{sline}', this.checked)">#{sline}</div>
EOF
end
io.close

#Read contents from commands.txt and show commands
io = open("commands.txt", "r:UTF-8")
title = io.gets
  print <<EOF
    <p class="commands">
      <h2>#{title}</h2>
EOF
while line = io.gets
  sline = line.chomp
    print <<EOF
<div><INPUT type="checkbox" name="selects" id="commandtcsubmit#{sline}" value=" " onclick="inserttcsubmit('commandtcsubmit#{sline}', this.checked);inserttcsubmit('commandtcOf#{sline}', this.checked)"><INPUT type="text" id="tcOf#{sline}" size="20" value="#{sline}" disabled="true"><INPUT type="text" id="commandtcOf#{sline}" size="30" disabled="true" value=""></div>
EOF
end
io.close

#add signature
print <<EOF
  <div>Signature<br><input type="text" name="Name"></div>
EOF

print <<EOFT
<p>
  <input type="submit" value="Submit">
  <input type="reset" value="Reset">
</p>
</form>
</p>
<textarea name="txrxLog" id="txrxLog" rows="20" cols="100">****Direwolf TX/RX log****</textarea>
</body>
EOFT
