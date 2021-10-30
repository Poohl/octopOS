# this skript compresses strings via RLE
# and builds a c-function to print them


import sys

def compress(lines):
    for l in lines:
        n = 1
        for i in range(1, len(l)):
            if l[i-1] == l[i]:
                n += 1
            else:
                char = l[i-1]
                if char == "'": char = "\\'"
                if n > 1:
                    print("    multiprint('",char,"', ", n, ");", sep='')
                    n = 1
                else:
                    print("    dbgu_put_byte('", char, "');", sep='')

        print("    dbgu_put_byte(", "'\\n'", ");", sep='')
            

if __name__ == "__main__":
    print("\n/* this function prints", sys.argv[1] ,"*/")
    print("void print_", sys.argv[1].split('/')[-1][:-4], "() {", sep='')
    with open(sys.argv[1]) as img:
            compress(img.readlines())
    print("}")