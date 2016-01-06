#!/usr/local/bin/ruby
# encoding: utf-8
require 'cgi'
cgi = CGI.new

ary = cgi.params["selects"]

ary = ary.join("\n")

data = open("vote_result.txt", "a:UTF-8")
data.write(ary + "\n")
data.close

print cgi.header("text/html; charset = utf-8")
print <<EOF                                                                     
<html>
Result<br>
#{ary}
<br>
Logging... completed
<a href="./enquete_form.rb">new logging</a>
<a href="./view_result.rb">logs</a>
</html>                                                                  
EOF
