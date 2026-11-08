"""Replace unlicensed legacy typefaces with OFL-licensed Noto Sans."""
from pathlib import Path
from urllib.request import urlopen

root = Path(__file__).resolve().parents[1]
font_url = "https://raw.githubusercontent.com/google/fonts/main/ofl/notosans/NotoSans%5Bwdth%2Cwght%5D.ttf"
license_url = "https://raw.githubusercontent.com/google/fonts/main/ofl/notosans/OFL.txt"
font = urlopen(font_url, timeout=30).read()
license_text = urlopen(license_url, timeout=30).read()
(root / "NotoSans-OFL.txt").write_bytes(license_text)
(root / "NotoSans.ttf").write_bytes(font)

