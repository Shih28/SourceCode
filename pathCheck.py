import os
for p in [
    "./assets/sound/levelmenu/hover.mp3"
]:
    print("\n")
    print(p, "->", os.path.abspath(p), os.path.exists(p))
