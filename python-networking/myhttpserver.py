from http.server import BaseHTTPRequestHandler, HTTPServer
import datetime
import json

HOSTNAME = "10.8.77.1"
PORT = 8888
DATETIME_FORMAT = "%A, %B %d at %H:%M"

def now_timestring(delta:int=7):
    tz = datetime.timezone(datetime.timedelta(hours=delta))
    return datetime.datetime.now(tz).strftime(DATETIME_FORMAT)

class MyServer(BaseHTTPRequestHandler):
    def response_html(self, status_code=200, html="<h1>Hello world</h1>"):
        try:
            self.send_response(status_code)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes("<html><head><title>My Server</title></head>", "utf-8"))
            self.wfile.write(bytes("<body>", "utf-8"))
            self.wfile.write(bytes(html, "utf-8"))
            self.wfile.write(bytes("</body></html>", "utf-8"))
        except Exception as e:
            print(e)
            self.reponse_error()

    def response_json(self, status_code=200, data={}):
        try:
            self.send_response(status_code)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(bytes(json.dumps(data), "utf-8"))
        except Exception as e:
            print(e)
            self.reponse_error()

    def reponse_error(self):
        self.response_html(500, "<h1>Internal Server Error</h1>")

    def response_notfound(self):
        self.response_html(404, "<h1>404 Not Found</h1>")

    def do_GET(self):
        if self.path == "/":
            self.response_html()
        elif self.path == "/date":
            self.response_html(html=now_timestring())
        elif self.path == "/date/paris":
            self.response_html(html=now_timestring(2))
        elif self.path == "/api/date":
            self.response_json(data={"Hanoi Time": now_timestring(), "Paris Time": now_timestring(2)})
        else:
            self.response_notfound()

if __name__ == "__main__":        
    webServer = HTTPServer((HOSTNAME, PORT), MyServer)
    print("Server started http://%s:%s" % (HOSTNAME, PORT))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")