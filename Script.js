// JavaScript source code
var commentsHidden = false;
var classHidden = false;
var functionHidden = false;

function toggleVisibilityComments(classname) {
    var elements = document.getElementsByClassName(classname);

    if (commentsHidden)
        commentsHidden = false;
    else
        commentsHidden = true;

    console.log(commentsHidden);
    for (var i = 0; i < elements.length; i++)
        elements[i].hidden = commentsHidden;


    var elementButton = document.getElementById("comments-button");
    console.log(elementButton.innerHTML);
    if (commentsHidden)
        elementButton.innerHTML = "Show comments";
    else
        elementButton.innerHTML = "Hide Comments"
}

function toggleVisibilityClass(classname) {
    var elements = document.getElementsByClassName(classname);

    if (classHidden)
        classHidden = false;
    else
        classHidden = true;

    console.log(classHidden);
    for (var i = 0; i < elements.length; i++)
        elements[i].hidden = classHidden;

    var elementsButton = document.getElementsByClassName("class-button");
    for (var i = 0; i < elementsButton.length; i++)
        if (classHidden)
            elementsButton[i].innerHTML = "+";
        else
            elementsButton[i].innerHTML = "-"

    var elementButton = document.getElementById("class-button");
    console.log(elementButton.innerHTML);
    if (classHidden)
        elementButton.innerHTML = "Show classes";
    else
        elementButton.innerHTML = "Hide classes"

}

function toggleVisibilityFunction(classname) {
    var elements = document.getElementsByClassName(classname);

    if (functionHidden)
        functionHidden = false;
    else
        functionHidden = true;

    console.log(functionHidden);
    for (var i = 0; i < elements.length; i++)
        elements[i].hidden = functionHidden;

    var elementsButton = document.getElementsByClassName("function-button");
    for (var i = 0; i < elementsButton.length; i++)
        if (functionHidden)
            elementsButton[i].innerHTML = "+";
        else
            elementsButton[i].innerHTML = "-"

    var elementButton = document.getElementById("function-button");
    console.log(elementButton.innerHTML);
    if (functionHidden)
        elementButton.innerHTML = "Show functions";
    else
        elementButton.innerHTML = "Hide functions"
}

function toggleVisibilityByID(idName) {
    var element = document.getElementById(idName);
    console.log(element.hidden);
    element.hidden = !element.hidden

    var elementButton = document.getElementById(idName + "-button");
    console.log(elementButton.innerHTML);
    if (elementButton.innerHTML == "+")
        elementButton.innerHTML = "-";
    else
        elementButton.innerHTML = "+"
}
