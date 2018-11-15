text = """
#############################
#px#x x#x x#x # x#x x#x x#x #
# x#x x#x x#x # x#x x#x x#x #
# xxx# x #xxx   xxx# x #xxx #
#  xx#x#x#xx  #  xx#x#x#xx  #
#x##r#b#b#r##x x##r#b#b#r##x#
#xx#  x#x  #xx#xx#  x#x  #xx#
# x#x x#x x#x # x#x x#x x#x #
# xxx# x #xxx   xxx# x #xxx #
#  xx#x#x#xx  #  xx#x#x#xx p#
#############################
""".strip()

#text = """
#######################
##p             #r#r#r#
######## ###### # # # #
##r             # # # #
######## ###### # # # #
##r             # # # #
######## ###### # # # #
##                    #
## # # # cccccc # # # #
## # # # cbbbbc # # # #
## # # # cb  bc # # # #
## # # # cb  bc # # # #
## # # # cbbbbc # # # #
## # # # cccccc # # # #
##                    #
## # # # ###### #######
## # # #             r#
## # # # ###### #######
## # # #             r#
## # # # ###### #######
##r#r#r#             p#
#######################
#""".strip()

conv = {
    "x": 0,
    "c": 2,
    "#": 1,
    "b": 5,
    "p": 3,
    "r": 7
}

with open("map.btm", "w") as f:
    for y, line in enumerate(text.split("\n")):
        for x, txt in enumerate(line):
            if txt.isdigit():
                f.write(f"3 {x} {y}\n")
            elif txt.strip():
                f.write(f"{conv[txt]} {x} {y}\n")
