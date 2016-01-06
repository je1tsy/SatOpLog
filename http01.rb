#!/usr/local/bin/ruby
# -*- coding: utf-8 -*-

require 'webrick'
include WEBrick

CGI_PATH = '/usr/local/bin/ruby'
DOC_PATH = File.join(Dir::pwd, "public_html/vote/")

logger = WEBrick::BasicLog.new(WEBrick::BasicLog::DEBUG)

server = HTTPServer.new(
  :Port => 8080,
  :DocumentRoot => File.join(Dir::pwd, "public_html"),
  :CGIInterpreter => CGI_PATH
)

#server.mount('/vote/enquete_form.cgi', WEBrick::HTTPServlet::CGIHandler,  '/vote/enquete_form.rb')

#server.mount('./*', WEBrick::HTTPServlet::CGIHandler, './*')

server.mount_proc('/vote') do |req, res|
  WEBrick::HTTPServlet::CGIHandler.new(server, DOC_PATH + req.path_info).service(req, res)
end

trap("INT"){s.shutdown}
server.start
