import os
for p in [
    "./assets/image/monsters/fire/1/BadGyaumal/walk/1.png"
]:
    print("\n")
    print(p, "->", os.path.abspath(p), os.path.exists(p))
