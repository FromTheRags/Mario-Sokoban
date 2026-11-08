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
    top, bottom = (PALETTE["sky"], (221, 244, 226)) if title else (PALETTE["sky"], (222, 241, 218))
    for y in range(h):
        a = y / max(1, h-1)
        col = tuple(round(top[i]*(1-a)+bottom[i]*a) for i in range(3))
        for x in range(w): pix[x, y] = col
    d = ImageDraw.Draw(im)
    # Distant hills and drifting puffy clouds.
    d.ellipse((-100, 245, 260, 420), fill=(164, 211, 160))
    d.ellipse((145, 220, 510, 420), fill=(148, 201, 143))
    d.ellipse((390, 250, 790, 430), fill=(167, 215, 158))
    for x, y, scale in ((75, 72, 1.0), (360, 112, .78), (570, 62, .9)):
        r = int(17*scale)
        d.ellipse((x, y+r, x+4*r, y+3*r), fill=(255,255,239), outline=(113,171,185), width=2)
        d.ellipse((x+r, y, x+3*r, y+3*r), fill=(255,255,239), outline=(113,171,185), width=2)
        d.ellipse((x+2*r, y+r//2, x+5*r, y+3*r), fill=(255,255,239), outline=(113,171,185), width=2)
        d.rectangle((x+r, y+2*r, x+4*r, y+3*r), fill=(255,255,239))
    # Layered garden trees with textured trunks and rounded leaf crowns.
    for x, base, scale in ((28, 440, 1.0), (175, 456, .82), (500, 444, 1.12), (625, 458, .88)):
        trunk_w=int(24*scale); crown=int(52*scale)
        d.rectangle((x+ crown//2-trunk_w//2, base-70*scale, x+crown//2+trunk_w//2, base+20), fill=(119,78,53), outline=PALETTE["ink"], width=2)
        d.line((x+crown//2, base-32*scale, x+5, base-65*scale), fill=(151,103,63), width=5)
        for ox,oy,rad,col in ((0,-78,crown,(66,137,87)),(26,-97,crown*.82,(81,157,92)),(-22,-96,crown*.78,(104,177,100)),(10,-120,crown*.65,(128,190,105))):
            d.ellipse((x+ox-rad/2,base+oy-rad/2,x+ox+rad/2,base+oy+rad/2),fill=col,outline=(48,103,72),width=2)
    # A stepped hedge and flowered grass give the game garden depth.
    d.rectangle((0,h-92,w,h), fill=(98,165,96))
    d.rectangle((0,h-52,w,h), fill=(70,137,79))
    for x in range(-12,w,58):
        d.ellipse((x,h-111,x+45,h-67),fill=(74,151,86),outline=(49,112,70),width=2)
        d.line((x+23,h-70,x+29,h-100),fill=(173,213,123),width=3)
        for fx in (x+12,x+34):
            d.ellipse((fx-3,h-40,fx+3,h-34),fill=(255,244,192))
    # Decorative goal crates at the garden edge.
    for x in (87, 555):
        y=h-95
        d.rectangle((x,y,x+40,y+40),fill=(164,102,58),outline=PALETTE["ink"],width=3)
        d.line((x+5,y+5,x+35,y+35),fill=(225,170,96),width=4)
        d.line((x+35,y+5,x+5,y+35),fill=(225,170,96),width=4)
        d.ellipse((x+10,y+10,x+30,y+30),outline=PALETTE["gold"],width=3)
    if title:
        # Keep the center clear for the title and two large menu buttons.
        d.rounded_rectangle((157, 35, 523, 129), radius=20, fill=(255,245,210), outline=PALETTE["ink"], width=5)
        d.rounded_rectangle((164, 42, 516, 122), radius=16, outline=(244,198,93), width=3)
    return im

def save(name, image):
    # SDL 1.2's built-in loader reads BMP only, so every asset uses real BMP
    # data and a .bmp filename. SDL3 also supports this format natively.
    image.save(OUT / name, format="BMP")

# A gold-and-ribbon award badge used by victory screens.
medal = Image.new("RGB", (68, 68), PALETTE["cream"])
md = ImageDraw.Draw(medal)
md.polygon(((18,42),(31,42),(27,67),(17,58)), fill=(202,83,79), outline=PALETTE["ink"])
md.polygon(((37,42),(50,42),(51,58),(40,67)), fill=(75,133,173), outline=PALETTE["ink"])
md.ellipse((8,3,60,55), fill=(211,145,50), outline=PALETTE["ink"], width=3)
md.ellipse((13,8,55,50), fill=(250,211,104), outline=(255,245,190), width=3)
md.ellipse((20,15,48,43), fill=(244,198,93), outline=(192,128,38), width=2)
for angle in range(0,360,45):
    cx=34+round(math.cos(math.radians(angle))*10); cy=29+round(math.sin(math.radians(angle))*10)
    md.ellipse((cx-2,cy-2,cx+2,cy+2),fill=(255,250,225))
md.ellipse((30,25,38,33),fill=(255,255,240),outline=(192,128,38))
save("medal.bmp", medal)

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

# Character PNGs have transparent backgrounds and are the runtime artwork on Windows.
# Matching BMPs remain available for SDL 1.2 platforms without WIC.
character_pngs = {}
for facing in ("down", "up", "left", "right", "editor"):
    sprite = tile("keeper", (68, 68), facing).convert("RGBA")
    pixels = sprite.load()
    for py in range(sprite.height):
        for px in range(sprite.width):
            if pixels[px, py][:3] == PALETTE["cream"]:
                pixels[px, py] = (0, 0, 0, 0)
    character_pngs[facing] = sprite
    sprite.save(OUT / f"keeper_{facing}.png", format="PNG", optimize=True)

# Use the protagonist for both the SDL window icon and the native executable icon.
icon_source = character_pngs["down"]
icon_bmp = Image.new("RGB", (68, 68), PALETTE["cream"])
icon_bmp.paste(icon_source, mask=icon_source.getchannel("A"))
icon_bmp.resize((32, 32), Image.Resampling.LANCZOS).save(ROOT / "caisse.bmp", format="BMP")
icon_source.save(ROOT / "caisse.ico", format="ICO", sizes=[(16,16),(32,32),(48,48),(64,64),(128,128)])

# A small reward badge fits in the corner of a level card.
Image.open(OUT / "medal.bmp").resize((22,22), Image.Resampling.LANCZOS).save(OUT / "medal_small.bmp", format="BMP")

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

