// Klaudia Nowak
// 297936

#include "request_handling.h"
#include "http_headers.h"

ssize_t recv_untill(int fd, u_int8_t *buffer, size_t buffer_size, unsigned int timeout)
{
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	bool separator_found = false;
	size_t total_bytes_read = 0;
	while (total_bytes_read <= 0)
	{
		fd_set descriptors;
		FD_ZERO(&descriptors);
		FD_SET(fd, &descriptors);
		int ready = select(fd + 1, &descriptors, NULL, NULL, &tv);
		if (ready < 0)
			ERROR("select error");
		if (ready == 0)
			return -2;

		ssize_t bytes_read = recv(fd, buffer, buffer_size, 0);
		if (bytes_read <= 0)
			return bytes_read;
		else
		{
			total_bytes_read = bytes_read;
		}
	}
	return total_bytes_read;
}

char *find_file_type(char *type)
{
	char *content_type;
	if (strcmp(type, ".png") == 0)
		content_type = png_response;
	else if (strcmp(type, ".html") == 0)
		content_type = html_response;
	else if (strcmp(type, ".txt") == 0)
		content_type = text_response;
	else if ((strcmp(type, ".css") == 0) || (strcmp(type, ".map") == 0))
		content_type = css_response;
	else if ((strcmp(type, ".jpg") == 0) || (strcmp(type, "jpeg") == 0))
		content_type = jpg_response;
	else if (strcmp(type, ".pdf") == 0)
		content_type = pdf_response;
	else
		content_type = other_response;
	return content_type;
}

char *set_header(char *host, char *path, char *response_header)
{

	int newSize = strlen(host) + strlen(path) + 1;
	char *full_path = (char *)malloc(newSize);
	sprintf(full_path, "%s%s", host, path);

	char *buffer;

	if (!(strstr(host, hostname_localhost)) && !(strstr(host, hostname_lab)))
	{
		// forbidden, because host not match defined domain names
		sprintf(response_header, "%s%s", response_header, status_403);
		return "";
	}
	if (strcmp(path, "/") == 0)
	{
		// 301 - move, if path point to whole folder
		sprintf(response_header, "%s%s", response_header, status_301);
		sprintf(response_header, "%sLocation: %s\r\n", response_header, default_page);
		int size = sizeof(response_header) / sizeof(response_header[0]);
		char *type = html_response;
		sprintf(response_header, "%s%s", response_header, type);
		return "";
	}
	else
	{
		// corect request
		sprintf(response_header, "%s%s", response_header, status_200);

		// File type
		char *file_type;
		if(! (file_type = strrchr(path, '.')))	{
			sprintf(response_header, "%s", status_404);
			return "";
		}
		sprintf(response_header, "%s%s", response_header, find_file_type(file_type));
	}
	return full_path;
}

int send_response(int sockfd, u_int8_t *buffer, char *header, size_t n)
{
	size_t h_left = strlen(header);
	ssize_t header_bytes_sent = send(sockfd, header, h_left, 0);
	h_left -= header_bytes_sent;
	size_t n_left = n;
	while (n_left > 0)
	{
		ssize_t bytes_sent = send(sockfd, buffer, n_left, 0);
		if (bytes_sent < 0)
			ERROR("send error");
		n_left -= bytes_sent;
		buffer += bytes_sent;
	}
	return n;
}

int create_response(int sockfd, u_int8_t *recv_buffer, char *response_text)
{
	FILE *stream;
	char *file_data;
	char *request = (char *)recv_buffer;
	char *first_line;
	first_line = strtok_r(request, "\n", &request);

	char *method = strtok_r(first_line, " ", &first_line);
	if (!strstr(method, "GET"))
	{
		// trash - not implemented
		sprintf(response_text, "%s%s", response_text, status_501);
		size_t h_left = strlen(response_text);
		ssize_t header_bytes_sent;
		if (header_bytes_sent = send(sockfd, response_text, h_left, 0) < 0)
		{
			ERROR("send error");
		};
		return 0;
	}
	char *path = strtok_r(first_line, " ", &first_line);
	char *http = strtok_r(first_line, " ", &first_line);

	if ((!strstr(http, "HTTP/1.1")) || (strstr(path, "../")))
	{
		// trash - not implemented
		sprintf(response_text, "%s%s", response_text, status_501);
		size_t h_left = strlen(response_text);
		ssize_t header_bytes_sent;
		if (header_bytes_sent = send(sockfd, response_text, h_left, 0) < 0)
		{
			ERROR("send error");
		};
		return 0;
	}

	char *host;
	char *ptr = "";
	do
	{
		host = strtok_r(request, "\n", &request);
		ptr = strstr(host, "Host");

	} while (host != ptr);
	strtok_r(host, ": ", &host);
	host = strtok(++host, ":");

	char *connection;
	do
	{
		connection = strtok_r(request, "\n", &request);
		ptr = strstr(connection, "Connection");

	} while (connection != ptr && ptr);
	strtok_r(connection, ": ", &connection);
	connection = strtok(connection, ":");
	if (strstr(connection, "closed"))
	{
		// close connection
		return 0;
	}

	char *full_path = set_header(host, path, response_text);
	if (strcmp(full_path, "") == 0)
	{
		size_t h_left = strlen(response_text);
		ssize_t header_bytes_sent;
		if (header_bytes_sent = send(sockfd, response_text, h_left, 0) < 0)
		{
			ERROR("send error");
		};
		return header_bytes_sent;
	}
	stream = fopen(full_path, "r");
	if (!stream)
	{

		// 404, because file not found
		sprintf(response_text, "%s", status_404);
		size_t h_left = strlen(response_text);

		ssize_t header_bytes_sent;
		if (header_bytes_sent = send(sockfd, response_text, h_left, 0) < 0)
		{
			ERROR("send error");
		};
		return header_bytes_sent;
	}

	fseek(stream, 0, SEEK_END);
	long s = ftell(stream);
	fseek(stream, 0, SEEK_SET);
	file_data = malloc(s);
	int count = fread(file_data, 1, s, stream);
	fclose(stream);

	sprintf(response_text, "%sContent-Length: %ld\r\n\r\n", response_text, s);

	int size = sizeof(response_text) / sizeof(response_text[0]);

	size_t h_left = sizeof(response_text);
	int sent_bytes = send_response(sockfd, file_data, response_text, s);
	return sent_bytes;
}

int receive_request(int client_fd, int client_fd2, u_int8_t *recv_buffer)
{

	ssize_t bytes_read = recv_untill(client_fd, recv_buffer, BUFFER_SIZE, 10);
	if (bytes_read == -2)
	{
		close(client_fd);
		return -2;
	}
	else if (bytes_read == -1)
	{
		ERROR("recv error");
	}
	else if (bytes_read == 0)
	{
		return 0;
	}
	else
	{
		recv_buffer[bytes_read] = 0;
		return bytes_read;
	}
}
