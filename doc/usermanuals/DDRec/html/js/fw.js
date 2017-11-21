// basic DOM functions
// based on http://blog.adtile.me/2014/01/16/a-dive-into-plain-javascript/
var q = document.querySelectorAll.bind(document);
Element.prototype.on = Element.prototype.addEventListener;
var createEl = function(el, cls, text){
  var x = document.createElement(el);
  x.setAttribute("class",cls);
  if(text) {
    var t = document.createTextNode(text);
    x.appendChild(t);
  }
  return x;
}
