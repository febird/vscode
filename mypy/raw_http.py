#!/usr/bin/python  
import socket  
import signal  
import errno
import os
from io import StringIO
  
def fileResponse(cfd,filename):
    f = open(filename,'rb')
    context = f.read()
    header = "%s %d\n\n" % (HTTP_HEADER_200,len(context))
    cfd.send(header.encode())
    cfd.send(context)
    cfd.send("\n\n".encode())
    f.close()

def dirResponse(cfd,dirname):
    dirlist = '<li><a href="../">..</a></li>'
    for filename in os.listdir(dirname):
        # remove '.' prefix so browser can provide absoulutely URI
        item = '<li><a href="%s/%s">%s</a></li>' % (dirname[1:len(dirname)],filename,filename)
        dirlist = dirlist + item
    context = HTTP_TEMPLATE_DIR % (dirname,dirlist)
    response = "%s %d\n\n%s\n\n" % (HTTP_HEADER_200,len(context),context)  
    cfd.send(response.encode())

def errorResponse(cfd):
    context = HTTP_TEMPLATE_404
    response = "%s %d\n\n%s\n\n" % (HTTP_HEADER_404,len(context),context)  
    cfd.send(response.encode())

def sigIntHander(signo,frame):
    print('get signo #',signo)
    global runflag  
    runflag = False  
    global lisfd  
    lisfd.shutdown(socket.SHUT_RD)  

def httpResponse(cfd,addr,req):
    content = ''
    reqs = StringIO(str(req,encoding='utf-8'))
    # add '.' to path for local file access
    path = '.'+reqs.readline().split(' ')[1]
    # remove last '/'
    if '/' == path[len(path)-1]:
        path = path[0:len(path)-1]
    reqs.close()
    if os.path.isdir(path):
        if os.path.isfile(path +'/'+ DEFAULT_HTML_FILE) :
            content = fileResponse(cfd,path +'/'+ DEFAULT_HTML_FILE)
        else:
            content = dirResponse(cfd,path)
    elif os.path.isfile(path):
        content = fileResponse(cfd,path)
    else:
        content = errorResponse(cfd)
        print(path + ' Not Found!')
 
HTTP_HEADER_200 = '''
HTTP/1.1 200 OK 
Context-Type: text/html 
Server: Python-mars version 1.0 
Context-Length: '''

HTTP_HEADER_404 = '''
HTTP/1.1 404 Not Found 
Context-Type: text/html 
Server: Python-mars version 1.0 
Context-Length: '''

HTTP_TEMPLATE_404 = '''
<html><head><title>404 Not Found</title></head>
<body><h1>404 Not Found</h1><p>Can not find your request on this server.</p>
<hr/><p>Servered by python-mars 0.1</p></body></html>'''

HTTP_TEMPLATE_DIR = '''
<html><head><title>List Dir</title></head>
<body><h1>List Dir %s</h1><p><ul>%s</ul></p>
<hr/><p>Servered by python-mars 0.1</p></body></html>'''

DEFAULT_HTML_FILE = "index.html"
strHost = "0.0.0.0"  
HOST = strHost #socket.inet_pton(socket.AF_INET,strHost)  
PORT = 8080 

lisfd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)  
lisfd.bind((HOST, PORT))  
lisfd.listen(2)  
print ("HTTP server is at: http://%s:%d/" % (HOST, PORT))
signal.signal(signal.SIGINT,sigIntHander)  
  
runflag = True  
while runflag:  
    try:  
        confd,addr = lisfd.accept()  
    except socket.error as e:  
        if e.errno == errno.EINTR:  
            print ('get a except EINTR')
        else:  
            raise  
        continue  
  
    if runflag == False:  
        break;  
  
    print ("connect by ",addr) 
    req = confd.recv(1024)  
    if req:    
        print(req)
        httpResponse(confd,addr,req)
    confd.close()  
else:  
    print('runflag#',runflag)  
  
print('Exit Server.') 