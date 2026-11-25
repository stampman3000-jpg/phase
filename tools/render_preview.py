#!/usr/bin/env python3
"""Render live 1280x628 visual previews of both CZboy pages + catalogue screenshot."""
import json
import os
import shutil
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKIN_DIR = os.path.join(ROOT, "plugins", "phase", "skins")
MANIFEST = os.path.join(ROOT, "tools", "ui_manifest.json")
OUT_DIR  = os.path.join(ROOT, "docs", "previews")
SHOT_DIR = os.path.join(ROOT, "screenshots")
os.makedirs(OUT_DIR, exist_ok=True)
os.makedirs(SHOT_DIR, exist_ok=True)


def get_frame(strip, frames, idx):
    im = Image.open(os.path.join(SKIN_DIR, strip)).convert("RGBA")
    fh = im.height // frames
    idx = max(0, min(frames - 1, idx))
    return im.crop((0, idx * fh, im.width, (idx + 1) * fh))


def draw_label(canvas, lb, txt):
    dr = ImageDraw.Draw(canvas)
    sz = max(11, int(round(lb["size"] * 0.72)))
    try:
        font = ImageFont.truetype("/System/Library/Fonts/HelveticaNeue.ttc", sz, index=1)
    except Exception:
        font = ImageFont.load_default()
    c = lb["color"]
    col = (int(c[2:4], 16), int(c[4:6], 16), int(c[6:8], 16), 255)
    bb = dr.textbbox((0, 0), txt, font=font)
    dr.text((lb["x"] + (lb["w"] - (bb[2] - bb[0])) // 2,
             lb["y"] + (lb["h"] - (bb[3] - bb[1])) // 2), txt, font=font, fill=col)


def main():
    with open(MANIFEST, "r", encoding="utf-8") as fh:
        m = json.load(fh)

    vals = {
        "line_mode": 2/3, "l1_wave1": 2/7, "l1_wave2": 5/8, "mod_mode": 0.0,
        "dcw_timbre": 0.28, "dcw_env": 0.76, "chorus": 0.34, "volume": 0.82,
        "dcw_att": 0.14, "dcw_dec": 0.42, "dcw_sus": 0.48, "dcw_rel": 0.22,
        "dca_att": 0.06, "dca_dec": 0.40, "dca_sus": 0.88, "dca_rel": 0.20,
        "l2_wave1": 0.0, "l2_wave2": 5/8, "l2_oct": 0.5, "l2_semi": 0.5,
        "voice_mode": 0.0, "l2_fine": (14 + 60) / 120.0, "pitch_env": 0.12, "pitch_dec": 0.14,
        "l2_mix": 0.90, "l2_dcw": 0.88, "l2_env_ofs": 0.58, "glide": 0.0,
        "vib_rate": 0.34, "vib_depth": 0.14, "space_send": 0.56, "space_decay": 0.76,
    }

    for pg, bg, out_name, wave_fi, lbl_txts in [
        ("page1", "bg_phase.png", "page1_phase.png", 4, ["Glassy Sine | 1+2 | V:4", "S-PL | POLY8"]),
        ("page2", "bg_line2.png", "page2_line2.png", 0, ["Glassy Sine | 1+2 | V:4", "S-PL | POLY8"]),
    ]:
        im = Image.open(os.path.join(SKIN_DIR, bg)).convert("RGBA")
        for st in m[pg].get("strips", []):
            if "wave_lcd" in st["id"]:
                fi = wave_fi
            elif "meter_l" in st["id"]:
                fi = 5
            else:
                fi = 6
            fr = get_frame(st["strip"], st["frames"], fi)
            im.alpha_composite(fr, (int(st["cx"] - st["w"]/2), int(st["cy"] - st["h"]/2)))
        for k in m[pg]["knobs"]:
            v = vals.get(k["id"], 0.50)
            fr = get_frame(k["strip"], k["frames"], int(round(v * (k["frames"] - 1))))
            im.alpha_composite(fr, (int(k["cx"] - k["w"]/2), int(k["cy"] - k["h"]/2)))
        for lb, txt in zip(m[pg].get("labels", []), lbl_txts):
            draw_label(im, lb, txt)
        out_path = os.path.join(OUT_DIR, out_name)
        im.convert("RGB").save(out_path)
        if pg == "page1":
            shutil.copy2(out_path, os.path.join(SHOT_DIR, "phase.png"))
    print(f"Rendered Phase previews to {OUT_DIR}/ and screenshots/phase.png")


if __name__ == "__main__":
    main()
