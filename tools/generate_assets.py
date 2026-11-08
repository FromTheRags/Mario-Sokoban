"""Generate the original, CC0 pixel-art assets used by Garden Sokoban."""
from pathlib import Path
from PIL import Image, ImageDraw
import math

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "sprites_mario_sokoban"
OUT.mkdir(parents=True, exist_ok=True)

PALETTE = {
    "ink": (29, 43, 61), "navy": (32, 55, 84), "sky": (130, 204, 218),
    "cream": (250, 239, 205), "green": (82, 147, 99), "leaf": (147, 190, 105),
    "wood": (167, 105, 61), "wood_hi": (219, 158, 87), "red": (202, 83, 79),
    "gold": (244, 198, 93), "white": (255, 250, 230), "blue": (75, 133, 173),
}

def fit_canvas(size, base):
    return Image.new("RGB", size, base)

def tile(kind, size=(68, 68)):
    w, h = size
    im = fit_canvas(size, PALETTE["cream"])
    d = ImageDraw.Draw(im)
    if kind == "wall":
        im.paste(PALETTE["navy"], (0, 0, w, h))
        for y in range(0, h, 17):
            d.line((0, y, w, y), fill=PALETTE["blue"], width=2)
            shift = 0 if (y // 17) % 2 == 0 else 16
            for x in range(-shift, w, 32): d.line((x, y, x, min(y+17,h)), fill=PALETTE["blue"], width=2)
        d.rectangle((5, 5, w-6, h-6), outline=PALETTE["sky"], width=2)
    elif kind == "crate":
        d.rounded_rectangle((5, 5, w-6, h-6), radius=7, fill=PALETTE["wood"], outline=PALETTE["ink"], width=3)
        d.line((12, 12, w-13, h-13), fill=PALETTE["wood_hi"], width=7)
        d.line((w-13, 12, 12, h-13), fill=PALETTE["wood_hi"], width=7)
        d.rectangle((w//2-4, 8, w//2+4, h-9), fill=PALETTE["wood"])
    elif kind == "target":
        d.ellipse((9, 9, w-10, h-10), fill=PALETTE["gold"], outline=PALETTE["ink"], width=3)
        d.ellipse((18, 18, w-19, h-19), outline=PALETTE["red"], width=5)
        d.ellipse((w//2-4, h//2-4, w//2+4, h//2+4), fill=PALETTE["red"])
    elif kind == "keeper":
        # A small, original garden keeper in four facing directions.
        d.ellipse((18, 3, w-19, 26), fill=PALETTE["green"], outline=PALETTE["ink"], width=2)
        d.rounded_rectangle((14, 19, w-15, h-5), radius=11, fill=PALETTE["blue"], outline=PALETTE["ink"], width=2)
        d.rectangle((20, h-12, 29, h-4), fill=PALETTE["ink"]); d.rectangle((w-29, h-12, w-20, h-4), fill=PALETTE["ink"])
        d.ellipse((25, 12, 29, 16), fill=PALETTE["white"]); d.ellipse((w-29, 12, w-25, 16), fill=PALETTE["white"])
        d.arc((w//2-7, 13, w//2+7, 23), 10, 170, fill=PALETTE["ink"], width=2)
    elif kind == "eraser":
        d.polygon(((13, h-11), (34, 10), (w-10, 27), (w-31, h-8)), fill=PALETTE["red"], outline=PALETTE["ink"])
        d.line((24, h-11, 45, 24), fill=PALETTE["white"], width=3)
    elif kind == "save":
        d.rounded_rectangle((12, 6, w-13, h-7), radius=5, fill=PALETTE["blue"], outline=PALETTE["ink"], width=3)
        d.rectangle((20, 10, w-21, 26), fill=PALETTE["cream"])
        d.rectangle((20, 36, w-21, h-12), fill=PALETTE["gold"])
    elif kind == "replay":
        d.arc((12, 12, w-12, h-12), 35, 320, fill=PALETTE["green"], width=7)
        d.polygon(((13, 15), (31, 13), (22, 30)), fill=PALETTE["green"])
    elif kind == "back":
        d.rounded_rectangle((4, 8, w-5, h-9), radius=9, fill=PALETTE["red"], outline=PALETTE["ink"], width=3)
        d.polygon(((12, h//2), (31, 16), (31, 28), (w-12, 28), (w-12, h-28), (31, h-28), (31, h-16)), fill=PALETTE["white"])
    elif kind == "button":
        d.rounded_rectangle((2, 5, w-3, h-6), radius=12, fill=PALETTE["green"], outline=PALETTE["ink"], width=3)
        d.line((13, 12, w-14, 12), fill=PALETTE["leaf"], width=3)
    else:
        d.rectangle((0, 0, w-1, h-1), fill=PALETTE["sky"])
    return im

def backdrop(size, title=False):
    w, h = size
    im = Image.new("RGB", size)
    pix = im.load()
    top, bottom = (PALETTE["sky"], PALETTE["cream"]) if title else (PALETTE["cream"], PALETTE["sky"])
    for y in range(h):
        a = y / max(1, h-1)
        col = tuple(round(top[i]*(1-a)+bottom[i]*a) for i in range(3))
        for x in range(w): pix[x, y] = col
    d = ImageDraw.Draw(im)
    # Repeating garden rows and oversized leaves are drawn from primitives.
    for x in range(-30, w, 74):
        d.ellipse((x, h-105, x+48, h-40), fill=PALETTE["green"])
        d.line((x+24, h-43, x+41, h-82), fill=PALETTE["leaf"], width=3)
    d.rectangle((0, h-30, w, h), fill=PALETTE["green"])
    if title:
        d.rounded_rectangle((w//2-155, 35, w//2+155, 111), radius=22, fill=PALETTE["cream"], outline=PALETTE["ink"], width=4)
        for x in range(w//2-116, w//2+117, 45):
            d.ellipse((x-8, 68, x+8, 84), fill=PALETTE["red"], outline=PALETTE["ink"])
    return im

def save(name, image):
    formats = {".png": "PNG", ".bmp": "BMP", ".gif": "GIF", ".jpg": "JPEG", ".jpeg": "JPEG"}
    fmt = formats[Path(name).suffix.lower()]
    if fmt == "JPEG": image = image.convert("RGB")
    image.save(OUT / name, format=fmt, optimize=(fmt == "PNG"), quality=94)

for filename, kind in {
    "caisse.jpg": "crate", "caisse_ok.jpg": "target", "caissecrea.jpg": "crate",
    "mur.jpg": "wall", "murcrea.jpg": "wall", "objectif.png": "target",
    "objectifcrea.png": "target", "mario_bas.gif": "keeper", "mario_haut.gif": "keeper",
    "mario_gauche.gif": "keeper", "mario_droite.gif": "keeper", "mariocrea.gif": "keeper",
    "replay.bmp": "replay", "retour.bmp": "back", "retour.gif": "back", "retour.png": "back",
    "filesave.png": "save", "gomme.bmp": "eraser", "jouer.png": "button",
}.items():
    save(filename, tile(kind))

# The executable window icon is generated from the same original crate motif.
tile("crate", (32, 32)).save(ROOT / "caisse.bmp", format="BMP")
tile("crate", (32, 32)).save(ROOT / "caisse.ico", format="ICO")

for filename in ("menu1.png", "nice.bmp", "creation.jpg", "dragon.jpg"):
    # Preserve the legacy window dimensions while replacing their artwork.
    old = OUT / filename
    try:
        with Image.open(old) as prior: size = prior.size
    except Exception: size = (680, 544)
    save(filename, backdrop(size, title=filename == "menu1.png"))

