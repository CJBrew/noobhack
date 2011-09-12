patterns = {
# styles: bold, dim, underline, blink, reverse
# colors: black, blue, cyan, green, magenta, red, white, yellow, brown, default
# format: "regex" (assume pre- and post- fixed with /b) : (style, [forecolor, [backcolor]])
    "blessed": ("bold", "green"),
    "uncursed" : ("dim", "yellow"),
    "cursed" : ("bold", "red", "yellow"),
    "cloak of displacement" : ("underline", "black", "cyan"),
    "[0-9a]+ potion[s]? of gain level" : ("blink", ), # note single length tuple requires a comma 
    "[0-9a]+ potion[s]? of gain energy" : ("reverse",)
}
