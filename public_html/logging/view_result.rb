#!/usr/local/bin/ruby
# encoding: utf-8
require 'cgi'
cgi = CGI.new

ary = Array.new

count = Hash.new(0)

data = open("log_data.txt", "r:UTF-8")

data.flock(File::LOCK_EX)
while line = data.gets
  line = line.chomp
  count[line] += 1
end
data.flock(File::LOCK_UN)
data.close

print cgi.header("text/html; charset = utf-8")
print <<EOF
  <html>
  Logs<br>
EOF

count.each{|key,value|
if key != "" then
  print "#{key} :  #{value} <br>"
end
}

print <<EOF
<br>
<a href="./main.rb">new logging</a>
</html>                                                                  
EOF
