SocketController = function(host, port) {
	this.socket = new WebSocket('ws://'+host+':'+port+'/','touchserver-protocol');

	this.socket.onmessage = this.onMessage;
}

	      
	      
SocketController.prototype.onMessage = function(e) {
	var a = e.data.split(',');
	if (a.length==3) {
    for (var i=0;i < 3; i++) { 
    $('.bar'+ i).css('background-color', a[i]);
   }
}  }

SocketController.prototype.send = function(id) {
	if (this.socket.readyState>0) this.socket.send(id);
}