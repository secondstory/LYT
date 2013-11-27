var Pixel = function(x, y, c, color, panel, localX,localY) {
	this.x = x-1;
	this.y = y-1;
	this.context = c;
	this.color = color;
	this.panel = panel;
	this.localX = localX;
  this.lcalY = localY;
	this.id = localY + '-' + localX + '-' + panel;
}
Pixel.prototype.update = function() {
	this.context.fillStyle = this.color;
	this.context.fillRect(this.x, this.y, 1, 1);
}