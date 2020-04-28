# my own HTTP Server

HTTP 1.1的web服务器，使用了持久连接
实现了线程池机制，默认30个线程
能检测HTTP请求报文是否是持久连接以及客户端TCP连接的关闭