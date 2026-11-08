"""Generate the original, CC0 pixel-art assets used by Garden Sokoban."""
from pathlib import Path
from PIL import Image, ImageDraw
import math

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "sprites_mario_sokoban"
OUT.mkdir(parents=True, exist_ok=True)
for obsolete in (
    "caisse.jpg", "caisse_ok.jpg", "caissecrea.jpg", "creation.jpg", "dragon.jpg",
    "filesave.png", "gomme.bmp", "jouer.png", "mario_bas.gif", "mario_droite.gif",
    "mario_gauche.gif", "mario_haut.gif", "mariocrea.gif", "menu1.png", "mur.jpg",
    "murcrea.jpg", "nice.bmp", "objectif.png", "objectifcrea.png", "replay.bmp",
    "retour.bmp", "retour.gif", "retour.png",
):
    (OUT / obsolete).unlink(missing_ok=True)

PALETTE = {
    "ink": (29, 43, 61), "navy": (32, 55, 84), "sky": (130, 204, 218),
    "cream": (250, 239, 205), "green": (82, 147, 99), "leaf": (147, 190, 105),
    "wood": (167, 105, 61), "wood_hi": (219, 158, 87), "red": (202, 83, 79),
    "gold": (244, 198, 93), "white": (255, 250, 230), "blue": (75, 133, 173),
}

def fit_canvas(size, base):
    return Image.new("RGB", size, base)

def tile(kind, size=(68, 68), facing="down"):
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
    elif kind in ("crate", "crate_target"):
        if kind == "crate_target":
            d.ellipse((3, 3, w-4, h-4), outline=PALETTE["gold"], width=5)
            d.ellipse((9, 9, w-10, h-10), outline=PALETTE["green"], width=2)
        d.rounded_rectangle((5, 5, w-6, h-6), radius=7, fill=PALETTE["wood"], outline=PALETTE["ink"], width=3)
        d.line((12, 12, w-13, h-13), fill=PALETTE["wood_hi"], width=7)
        d.line((w-13, 12, 12, h-13), fill=PALETTE["wood_hi"], width=7)
        d.rectangle((w//2-4, 8, w//2+4, h-9), fill=PALETTE["wood"])
    elif kind == "target":
        d.ellipse((8, 8, w-9, h-9), fill=(244, 198, 93, 90), outline=PALETTE["gold"], width=4)
        d.ellipse((17, 17, w-18, h-18), outline=PALETTE["green"], width=4)
        for angle in range(0, 360, 45):
            cx = w//2 + round(math.cos(math.radians(angle))*13)
            cy = h//2 + round(math.sin(math.radians(angle))*13)
            d.ellipse((cx-4, cy-4, cx+4, cy+4), fill=PALETTE["white"], outline=PALETTE["gold"])
        d.ellipse((w//2-5, h//2-5, w//2+5, h//2+5), fill=PALETTE["red"], outline=PALETTE["ink"])
    elif kind == "keeper":
        # Original retro-platform gardener, with warm cap, moustache and tool belt.
        d.ellipse((20, 8, 48, 39), fill=(242, 190, 137), outline=PALETTE["ink"], width=2)
        d.rectangle((18, 19, 23, 31), fill=(112, 63, 48))
        d.rectangle((45, 19, 50, 31), fill=(112, 63, 48))
        d.rounded_rectangle((16, 7, 50, 22), radius=7, fill=(227, 112, 55), outline=PALETTE["ink"], width=2)
        d.rectangle((12, 18, 53, 24), fill=(246, 177, 76), outline=PALETTE["ink"], width=2)
        eye_x = 27 if facing == "left" else 40 if facing == "right" else 29
        if facing not in ("up",):
            d.rectangle((eye_x, 24, eye_x+3, 29), fill=PALETTE["ink"])
            d.rectangle((eye_x+7, 24, eye_x+10, 29), fill=PALETTE["ink"])
            d.arc((29, 28, 42, 37), 10, 160, fill=(92, 53, 42), width=3)
        d.rounded_rectangle((14, 35, 54, 58), radius=7, fill=(47, 139, 143), outline=PALETTE["ink"], width=3)
        d.rectangle((22, 38, 29, 54), fill=(226, 155, 68), outline=PALETTE["ink"])
        d.rectangle((39, 38, 46, 54), fill=(226, 155, 68), outline=PALETTE["ink"])
        d.ellipse((23, 43, 28, 48), fill=PALETTE["gold"], outline=PALETTE["ink"])
        d.ellipse((40, 43, 45, 48), fill=PALETTE["gold"], outline=PALETTE["ink"])
        d.rectangle((10, 57, 30, 64), fill=(90, 74, 56), outline=PALETTE["ink"], width=2)
        d.rectangle((38, 57, 58, 64), fill=(90, 74, 56), outline=PALETTE["ink"], width=2)
        d.rectangle((31, 46, 37, 56), fill=(186, 105, 54), outline=PALETTE["ink"])
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
    elif kind == "floor":
        d.rectangle((0, 0, w-1, h-1), fill=(232, 220, 181))
        d.rectangle((2, 2, w-3, h-3), outline=(212, 201, 165), width=1)
        for x, y in ((12, 16), (48, 40), (26, 57), (57, 10)):
            d.ellipse((x, y, x+3, y+3), fill=PALETTE["green"])
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
    # SDL 1.2's built-in loader reads BMP only, so every asset uses real BMP
    # data and a .bmp filename. SDL3 also supports this format natively.
    image.save(OUT / name, format="BMP")

for filename, kind in {
    "floor.bmp": "floor",
    "crate.bmp": "crate", "crate_target.bmp": "crate_target", "crate_editor.bmp": "crate",
    "wall.bmp": "wall", "wall_editor.bmp": "wall", "target.bmp": "target",
    "target_editor.bmp": "target", "keeper_down.bmp": "keeper", "keeper_up.bmp": "keeper",
    "keeper_left.bmp": "keeper", "keeper_right.bmp": "keeper", "keeper_editor.bmp": "keeper",
    "retry.bmp": "replay", "back.bmp": "back", "save.bmp": "save",
    "eraser.bmp": "eraser", "play.bmp": "button",
}.items():
    save(filename, tile(kind, facing=filename.removeprefix("keeper_").removesuffix(".bmp") if filename.startswith("keeper_") else "down"))

# The executable window icon is generated from the same original crate motif.
tile("crate", (32, 32)).save(ROOT / "caisse.bmp", format="BMP")
tile("crate", (32, 32)).save(ROOT / "caisse.ico", format="ICO")

for filename in ("menu.bmp", "garden.bmp", "editor.bmp", "garden_detail.bmp"):
    # The SDL 1.2 menu fills the 680x544 window. Keep the generated screen
    # full-size so the artwork never crops the visible choices.
    old = OUT / filename
    try:
        with Image.open(old) as prior: size = prior.size
    except Exception: size = (680, 544)
    if filename == "menu.bmp":
        size = (680, 544)
    image = backdrop(size, title=filename == "menu.bmp")
    if filename == "menu.bmp":
        draw = ImageDraw.Draw(image)
        draw.rounded_rectangle((176, 210, 480, 255), radius=11, fill=PALETTE["green"], outline=PALETTE["ink"], width=4)
        draw.rounded_rectangle((176, 257, 480, 302), radius=11, fill=PALETTE["blue"], outline=PALETTE["ink"], width=4)
        draw.line((192, 217, 464, 217), fill=PALETTE["leaf"], width=3)
        draw.line((192, 264, 464, 264), fill=PALETTE["sky"], width=3)
    save(filename, image)

