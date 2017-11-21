document.addEventListener("DOMContentLoaded", function() {
    // Code
    // select local TOC
    var menu = q(".sectionTOCS")[0];
    var menuopened = "^^^";
    var m = createEl("a","menu","");
    var toolbar = createEl("div","toolbar");
    var sidebar = createEl("div","sidebar","");
    var crosslinks = q(".crosslinks")[0];
    var setText = function(node,a){
      var text = document.createTextNode(a);
      node.removeChild(node.childNodes[0]);
      node.appendChild(text);
    }
    // pass reference to new container and crosslinks
    var transfromCrosslinks = function(newcrosslinks,el){
      // find all crosslinks 
      var links = el.querySelectorAll("a");
      var newcrs = {}
     // make table with link text as name
     for(i=0;i!=links.length;i++){
          var text = links[i].firstChild.nodeValue;
          newcrs[text] = links[i];
      }
      // select next, prev and up nodes and replace text with unicode arrows
      // is a node doesn't exist, create span with blanklink class
      var next = newcrs["next"] || createEl("span","blanklink",">")
      setText(next,"⇨");
      var prev = newcrs["prev"]|| createEl("span","blanklink",">")
      setText(prev,"⇦");
      setText(newcrs["up"],"⇧");
      // add crosslinks to a toolbar
      newcrosslinks.appendChild(prev);
      newcrosslinks.appendChild(newcrs["up"]);
      newcrosslinks.appendChild(next);
      crosslinks.remove();
      return newcrosslinks;
    }
    // add elements to the sidebar depending on existence of a local TOC and 
    // crosslinks
    if(crosslinks){
      transfromCrosslinks(toolbar,crosslinks);
      // add sidebat only if it doesn't exist already
      if(!menu) sidebar.appendChild(toolbar);
      //m.appendChild(newcrosslinks);
    }
    if(menu) { 
      toolbar.appendChild(m);
      sidebar.appendChild(toolbar);
      sidebar.appendChild(menu);
    }
    q("body")[0].appendChild(sidebar);
    
 }, false);
