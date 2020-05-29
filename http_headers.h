char hostname_localhost[] = "localhost";
char hostname_lab[] = "lab108-18";
char default_page[] = "index.html";

char png_response[] = "Content-Type: image/png\r\n";
char html_response[] = "Content-Type: text/html; charset=UTF-8\r\n";
char text_response[] = "Content-Type: text/plain; charset=UTF-8\r\n";
char css_response[] = "Content-Type: text/css;\r\n\r\n";
char jpg_response[] = "Content-Type: image/jpeg;\r\n\r\n";
char pdf_response[] = "Content-Type: application/pdf; charset=UTF-8\r\n\r\n";
char other_response[] = "Content-Type: application/octet-stream; charset=UTF-8\r\n\r\n";

char status_200[] = "HTTP/1.1 200 OK:\r\n";
char status_301[] = "HTTP/1.1 301 Moved Permanently:\r\n";
char status_403[] = "HTTP/1.1 403 Forbidden:\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                    "<!DOCTYPE html><html><head><title>403 Forbidden</title>"
                    "<body><h1>403 Forbidden:</h1></body></html>\r\n";;

char status_404[] = "HTTP/1.1 404 Not Found:\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                    "<!DOCTYPE html><html><head><title>404 Not Found</title>"
                    "<body><h1>404 Not Found:</h1></body></html>\r\n";

char status_501[] = "HTTP/1.1 501 Not Implemented: \r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                    "<!DOCTYPE html><html><head><title>501 Not Implemented</title>"
                    "<body><h1>501 Not Implemented: </h1></body></html>\r\n";