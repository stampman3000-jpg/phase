#!/usr/bin/env python3
"""Procedural 1984 Casio CZ-Series Phase Distortion Hardware UI generator for CZboy.

Aesthetic:
  - Satin charcoal-slate 1984 Casio CZ polycarbonate console deck with micro-stipple texture
  - Recessed gunmetal side rails with signature Casio Aqua-Teal pinstripe inlay
  - Iconic Casio CZ colour-coded section strips (Aqua-Teal DCO, Signal-Coral DCW,
    Warm Amber-Gold DCA, Ice-Silver Output/Voice)
  - Backlit olive-mint Casio dot-matrix LCD bezel with live 8-waveform Phase Distortion
    vector scope, preset/voice telemetry readout, and dual stereo LCD level meters
  - 4x-supersampled fluted hardware rotary knobs + calibrated vertical ADSR faders
"""
import json
import math
import os
import random
from PIL import Image, ImageDraw, ImageFilter, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKIN_DIR = os.path.join(ROOT, "plugins", "czboy", "skins")
MANIFEST = os.path.join(ROOT, "tools", "ui_manifest.json")

W, H = 1280, 628
SS = 2

# 1984 Casio CZ-101 / CZ-1000 / CZ-5000 Hardware Palette
IVORY      = (236, 238, 232, 255)
IVORY_DIM  = (164, 170, 174, 255)
DARK_INK   = (20, 24, 28, 255)
RULE_LINE  = (110, 122, 130, 255)

CZ_TEAL    = (38, 176, 176, 255)
CZ_TEAL_HI = (78, 214, 210, 255)
CZ_CORAL   = (220, 74, 68, 255)
CZ_CORAL_H = (244, 106, 94, 255)
CZ_AMBER   = (224, 166, 48, 255)
CZ_AMBER_H = (244, 196, 82, 255)
CZ_SILVER  = (176, 188, 196, 255)

# Backlit Olive-Mint Casio LCD colours
LCD_BG     = (138, 166, 118, 255)
LCD_BG_DK  = (112, 140, 96, 255)
LCD_INK    = (24, 42, 26, 255)
LCD_GHOST  = (116, 144, 98, 255)
LCD_GRID   = (124, 152, 104, 255)


def load_font(px, bold=True):
    candidates = (
        ["/System/Library/Fonts/HelveticaNeue.ttc", "/System/Library/Fonts/Supplemental/Arial Bold.ttf"]
        if bold else
        ["/System/Library/Fonts/HelveticaNeue.ttc", "/System/Library/Fonts/Supplemental/Arial.ttf"]
    )
    for p in candidates:
        if os.path.exists(p):
            try:
                return ImageFont.truetype(p, int(px), index=1 if (p.endswith(".ttc") and bold) else 0)
            except Exception:
                try:
                    return ImageFont.truetype(p, int(px))
                except Exception:
                    pass
    return ImageFont.load_default()


def draw_text(img, text, x, y, size_px, fill=IVORY, align="center", bold=True, track=0.05):
    font = load_font(size_px * SS, bold=bold)
    tmp = Image.new("RGBA", (1, 1), (0, 0, 0, 0))
    tdr = ImageDraw.Draw(tmp)
    chars = list(text)
    widths = []
    for ch in chars:
        bb = tdr.textbbox((0, 0), ch, font=font)
        widths.append(bb[2] - bb[0])
    pad_tr = int(round(size_px * SS * track))
    total_w = sum(widths) + pad_tr * max(0, len(chars) - 1)
    if align == "center":
        cx = x - total_w / 2.0
    elif align == "right":
        cx = x - total_w
    else:
        cx = x
    cy = y - (size_px * SS) * 0.56
    dr = ImageDraw.Draw(img)
    for ch, cw in zip(chars, widths):
        dr.text((cx, cy), ch, font=font, fill=fill)
        cx += cw + pad_tr


def make_cz_chassis(seed=1984):
    """1984 Casio CZ matte charcoal-slate console deck with teal pinstripe side cheeks."""
    w, h = W * SS, H * SS
    rng = random.Random(seed)
    base = Image.new("RGBA", (w, h), (30, 34, 39, 255))
    dr = ImageDraw.Draw(base)

    # Subtle top-down studio light across satin slate faceplate
    for y in range(h):
        t = y / float(h)
        d = int(round(7.5 * (1.0 - abs(t - 0.22) * 1.45)))
        c = (max(18, min(52, 31 + d)), max(21, min(56, 35 + d)), max(25, min(62, 41 + d)), 255)
        dr.line([(0, y), (w, y)], fill=c)

    # Fine satin polycarbonate stipple
    grain = Image.new("RGBA", (w // 2, h // 2), (0, 0, 0, 0))
    px = grain.load()
    for gy in range(h // 2):
        for gx in range(w // 2):
            v = rng.randint(-5, 5)
            if v > 0:
                px[gx, gy] = (230, 240, 244, v * 2)
            elif v < 0:
                px[gx, gy] = (6, 8, 10, -v * 3)
    grain = grain.resize((w, h), Image.NEAREST)
    base = Image.alpha_composite(base, grain)
    dr = ImageDraw.Draw(base)

    # Molded gunmetal side cheeks + Casio Aqua-Teal pinstripe rails
    rail_w = 16 * SS
    for side in (0, 1):
        x0 = 0 if side == 0 else w - rail_w
        for i in range(rail_w):
            u = i / float(rail_w - 1)
            if side == 1:
                u = 1.0 - u
            lit = math.sin(u * math.pi)
            r = int(24 + 26 * lit)
            g = int(28 + 30 * lit)
            b = int(34 + 34 * lit)
            dr.line([(x0 + i, 0), (x0 + i, h)], fill=(r, g, b, 255))
        # Teal vertical inlay stripe along inner edge of side rail
        ix = (rail_w - 3 * SS) if side == 0 else (w - rail_w + 1 * SS)
        dr.rectangle([ix, 10 * SS, ix + 2 * SS, h - 10 * SS], fill=(38, 176, 176, 210))

    # Top and bottom anodized bezel trim
    dr.rectangle([rail_w, 0, w - rail_w, 5 * SS], fill=(48, 54, 62, 255))
    dr.line([(rail_w, 5 * SS), (w - rail_w, 5 * SS)], fill=(14, 16, 19, 255), width=SS)
    dr.rectangle([rail_w, h - 6 * SS, w - rail_w, h], fill=(20, 23, 27, 255))
    dr.line([(rail_w, h - 6 * SS), (w - rail_w, h - 6 * SS)], fill=(64, 72, 82, 255), width=SS)

    # Subtle recessed header band behind logo & LCD
    dr.rectangle([rail_w + 4 * SS, 8 * SS, w - rail_w - 4 * SS, 112 * SS], fill=(22, 26, 31, 255))
    dr.line([(rail_w + 4 * SS, 112 * SS), (w - rail_w - 4 * SS, 112 * SS)], fill=(38, 176, 176, 180), width=2 * SS)
    dr.line([(rail_w + 4 * SS, 115 * SS), (w - rail_w - 4 * SS, 115 * SS)], fill=(220, 74, 68, 150), width=SS)

    return base


def draw_lcd_bezel(img, x, y, w, h, title=None):
    """Draw a recessed 1984 Casio backlit olive-mint LCD window."""
    dr = ImageDraw.Draw(img)
    x0, y0, x1, y1 = int(x * SS), int(y * SS), int((x + w) * SS), int((y + h) * SS)
    rad = 6 * SS

    # Outer dark chamfer frame
    dr.rounded_rectangle([x0 - 3 * SS, y0 - 3 * SS, x1 + 3 * SS, y1 + 3 * SS], radius=rad + 2 * SS,
                         fill=(14, 17, 20, 255), outline=(66, 74, 84, 255), width=SS)
    # Olive-mint backlit LCD panel with subtle vertical gradient
    lcd = Image.new("RGBA", (x1 - x0, y1 - y0), LCD_BG)
    ldr = ImageDraw.Draw(lcd)
    lh = y1 - y0
    lw = x1 - x0
    for yy in range(lh):
        t = yy / float(max(1, lh - 1))
        d = int(round(10 * (1.0 - abs(t - 0.35) * 1.5)))
        ldr.line([(0, yy), (lw, yy)], fill=(
            max(80, min(175, LCD_BG[0] + d)),
            max(100, min(195, LCD_BG[1] + d)),
            max(70, min(145, LCD_BG[2] + d)),
            255,
        ))
    # Subtle inner LCD shadow from glass bezel
    ldr.rectangle([0, 0, lw - 1, 3 * SS], fill=(76, 98, 66, 140))
    ldr.rectangle([0, 0, 3 * SS, lh - 1], fill=(76, 98, 66, 110))
    ldr.rectangle([0, lh - 2 * SS, lw - 1, lh - 1], fill=(168, 194, 148, 90))
    img.alpha_composite(lcd, (x0, y0))

    dr = ImageDraw.Draw(img)
    dr.rounded_rectangle([x0, y0, x1, y1], radius=rad // 2, outline=(28, 38, 28, 220), width=SS)
    if title:
        draw_text(img, title, (x + 8) * SS, (y + 10) * SS, 8.5, fill=(34, 54, 34, 220), align="left", bold=True, track=0.10)


def draw_cz_header(img, subtitle):
    """Draw the iconic 1984 Casio CZboy nameplate + recessed backlit LCD strip."""
    dr = ImageDraw.Draw(img)

    # Brand block (left)
    draw_text(img, "CZ", 38 * SS, 33 * SS, 30, fill=IVORY, align="left", bold=True, track=0.02)
    draw_text(img, "boy", 86 * SS, 35 * SS, 27, fill=CZ_TEAL_HI, align="left", bold=True, track=0.02)

    # Classic dual Casio accent bars next to logo
    dr.rectangle([176 * SS, 22 * SS, 330 * SS, 27 * SS], fill=CZ_TEAL)
    dr.rectangle([176 * SS, 30 * SS, 294 * SS, 34 * SS], fill=CZ_CORAL)
    draw_text(img, "PD SYNTH", 176 * SS, 45 * SS, 9.5, fill=IVORY_DIM, align="left", bold=True, track=0.14)
    draw_text(img, subtitle, 38 * SS, 57 * SS, 8.5, fill=IVORY_DIM, align="left", bold=True, track=0.08)

    # Preset & Voice LCD (left-bottom inside header)
    draw_lcd_bezel(img, 36, 68, 296, 36)

    # Center Waveform Scope LCD (live 8-frame PD waveform plot)
    draw_lcd_bezel(img, 356, 18, 438, 86)

    # Right Status + Stereo Meter LCD
    draw_lcd_bezel(img, 818, 18, 424, 86, title="PHASE DISTORTION MONITOR")
    # Stereo L / R meter legend inside right LCD
    draw_text(img, "L", 1148 * SS, 96 * SS, 8.5, fill=LCD_INK, align="center", bold=True)
    draw_text(img, "R", 1188 * SS, 96 * SS, 8.5, fill=LCD_INK, align="center", bold=True)
    for db, yy in [("0", 30), ("-6", 46), ("-12", 62), ("-24", 78)]:
        draw_text(img, db, 1214 * SS, yy * SS, 7.2, fill=(38, 58, 38, 220), align="left", bold=True)


def draw_section_card(img, x, y, w, h, title, accent, tag=None):
    """Draw a crisp 1984 Casio CZ silkscreened hardware module bay."""
    dr = ImageDraw.Draw(img)
    x0, y0, x1, y1 = int(x * SS), int(y * SS), int((x + w) * SS), int((y + h) * SS)
    rad = 8 * SS

    # Subtle recessed panel plate
    dr.rounded_rectangle([x0, y0, x1, y1], radius=rad,
                         fill=(25, 29, 34, 255), outline=(62, 70, 78, 255), width=SS)

    # Colour-coded header strip
    hh = 28 * SS
    dr.rounded_rectangle([x0 + SS, y0 + SS, x1 - SS, y0 + hh], radius=rad - SS, fill=accent)
    dr.rectangle([x0 + SS, y0 + hh - 8 * SS, x1 - SS, y0 + hh], fill=accent)

    # Dark diagonal tech bars on right of header (classic 1984 Casio graphic touch)
    for i in range(3):
        bx = x1 - (14 + i * 9) * SS
        poly = [(bx, y0 + hh), (bx + 5 * SS, y0 + hh), (bx + 11 * SS, y0 + SS), (bx + 6 * SS, y0 + SS)]
        dr.polygon(poly, fill=(24, 28, 32, 190))

    ink = DARK_INK if (accent[0] * 0.3 + accent[1] * 0.59 + accent[2] * 0.11) > 122 else IVORY
    draw_text(img, title, (x + 12) * SS, (y + 14.5) * SS, 12.0, fill=ink, align="left", bold=True, track=0.10)
    if tag:
        draw_text(img, tag, (x + w - 42) * SS, (y + 14.5) * SS, 9.0, fill=ink, align="right", bold=True, track=0.06)


def choice_ticks(labels):
    n = len(labels)
    if n <= 1:
        return [(0.0, labels[0])]
    return [(i / float(n - 1), lb) for i, lb in enumerate(labels)]


def draw_knob_scale(img, dr, cx, cy, r_knob, label, kind="pot", ticks=None,
                    start_deg=-135, end_deg=135, accent=CZ_TEAL, label_y_offset=64):
    cx_s, cy_s = cx * SS, cy * SS
    r_in = (r_knob + 5.5) * SS
    r_out = (r_knob + 11.5) * SS
    r_lbl = (r_knob + 22.0) * SS

    def pt(deg, rad):
        a = math.radians(deg - 90.0)
        return (cx_s + rad * math.cos(a), cy_s + rad * math.sin(a))

    if kind == "choice" and ticks:
        for norm, txt in ticks:
            deg = start_deg + norm * (end_deg - start_deg)
            p0 = pt(deg, r_in)
            p1 = pt(deg, r_out + 1.5 * SS)
            dr.line([p0, p1], fill=accent, width=int(round(1.8 * SS)))
            lx, ly = pt(deg, r_lbl + 2 * SS)
            draw_text(img, txt, lx, ly, 8.2, fill=IVORY, align="center", bold=True, track=0.02)
    else:
        n_ticks = 11
        for i in range(n_ticks):
            u = i / float(n_ticks - 1)
            deg = start_deg + u * (end_deg - start_deg)
            major = (i in (0, 5, 10))
            col = accent if (kind == "bipolar" and i == 5) else (IVORY if major else RULE_LINE)
            dr.line([pt(deg, r_in), pt(deg, r_out if major else r_out - 2.5 * SS)],
                    fill=col, width=int(round((1.7 if major else 1.1) * SS)))
        if kind == "bipolar":
            lx0, ly0 = pt(start_deg, r_lbl)
            lx1, ly1 = pt(0, r_lbl - 2 * SS)
            lx2, ly2 = pt(end_deg, r_lbl)
            draw_text(img, "-", lx0, ly0, 9.5, fill=IVORY_DIM, align="center", bold=True)
            draw_text(img, "0", lx1, ly1, 8.5, fill=accent, align="center", bold=True)
            draw_text(img, "+", lx2, ly2, 9.5, fill=IVORY_DIM, align="center", bold=True)
        else:
            lx0, ly0 = pt(start_deg, r_lbl)
            lx1, ly1 = pt(0, r_lbl - 2 * SS)
            lx2, ly2 = pt(end_deg, r_lbl)
            draw_text(img, "0", lx0, ly0, 8.2, fill=IVORY_DIM, align="center", bold=True)
            draw_text(img, "5", lx1, ly1, 8.0, fill=IVORY_DIM, align="center", bold=True)
            draw_text(img, "10", lx2, ly2, 8.2, fill=IVORY_DIM, align="center", bold=True)

    draw_text(img, label, cx_s, (cy + label_y_offset) * SS, 10.6, fill=IVORY, align="center", bold=True, track=0.07)


def draw_fader_scale(img, dr, cx, cy, fw, fh, label, top_lbl="10", bot_lbl="0", accent=CZ_CORAL):
    cx_s, cy_s = cx * SS, cy * SS
    half_h = (fh * 0.41) * SS
    x_left_in = (cx - fw * 0.44) * SS
    x_right_in = (cx + fw * 0.44) * SS
    for i in range(11):
        u = i / 10.0
        yy = cy_s + half_h - u * (2.0 * half_h)
        major = (i in (0, 5, 10))
        tlen = (9.5 if major else 5.5) * SS
        col = accent if i == 10 else (IVORY if major else RULE_LINE)
        w_ln = int(round((1.6 if major else 1.0) * SS))
        dr.line([(x_left_in - tlen, yy), (x_left_in - 2 * SS, yy)], fill=col, width=w_ln)
        dr.line([(x_right_in + 2 * SS, yy), (x_right_in + tlen, yy)], fill=col, width=w_ln)

    draw_text(img, top_lbl, cx_s, (cy - fh * 0.56 - 11) * SS, 8.4, fill=accent, align="center", bold=True, track=0.04)
    draw_text(img, bot_lbl, cx_s, (cy + fh * 0.56 + 10) * SS, 8.4, fill=IVORY_DIM, align="center", bold=True, track=0.04)
    draw_text(img, label, cx_s, (cy + fh * 0.56 + 32) * SS, 10.6, fill=IVORY, align="center", bold=True, track=0.07)


def make_knob_strip(filename, sz, frames, cap_rgb, start_deg=-135, end_deg=135, indicator_rgb=(245, 248, 244)):
    """Render 4x-supersampled fluted 1984 Casio hardware knob filmstrip."""
    os_f = 4
    s = sz * os_f
    strip = Image.new("RGBA", (sz, sz * frames), (0, 0, 0, 0))
    cx = cy = s / 2.0
    r_base = s * 0.44
    r_cap = s * 0.28

    for fi in range(frames):
        u = fi / float(max(1, frames - 1))
        deg = start_deg + u * (end_deg - start_deg)
        rad = math.radians(deg - 90.0)

        im = Image.new("RGBA", (s, s), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)

        # Soft drop shadow
        sh = Image.new("RGBA", (s, s), (0, 0, 0, 0))
        sdr = ImageDraw.Draw(sh)
        sdr.ellipse([cx - r_base, cy - r_base + 5 * os_f, cx + r_base, cy + r_base + 5 * os_f], fill=(0, 0, 0, 145))
        sh = sh.filter(ImageFilter.GaussianBlur(radius=3.0 * os_f))
        im = Image.alpha_composite(im, sh)
        dr = ImageDraw.Draw(im)

        # Dark charcoal outer skirt + metallic chamfer ring
        dr.ellipse([cx - r_base, cy - r_base, cx + r_base, cy + r_base],
                   fill=(24, 27, 31, 255), outline=(72, 80, 88, 255), width=2 * os_f)
        # 18 fluted ridges around skirt
        for fl in range(18):
            a = rad + fl * (2.0 * math.pi / 18.0)
            p0 = (cx + (r_cap + 2 * os_f) * math.cos(a), cy + (r_cap + 2 * os_f) * math.sin(a))
            p1 = (cx + (r_base - 2 * os_f) * math.cos(a), cy + (r_base - 2 * os_f) * math.sin(a))
            shade = int(42 + 18 * math.cos(a + 0.8))
            dr.line([p0, p1], fill=(shade, shade + 3, shade + 6, 220), width=int(2.2 * os_f))

        # Inner recessed groove
        dr.ellipse([cx - r_cap - 2.5 * os_f, cy - r_cap - 2.5 * os_f,
                    cx + r_cap + 2.5 * os_f, cy + r_cap + 2.5 * os_f],
                   fill=(16, 18, 21, 255))

        # Anodized coloured cap with studio top highlight
        cr, cg, cb = cap_rgb
        dr.ellipse([cx - r_cap, cy - r_cap, cx + r_cap, cy + r_cap], fill=(cr, cg, cb, 255))
        dr.arc([cx - r_cap + os_f, cy - r_cap + os_f, cx + r_cap - os_f, cy + r_cap - os_f],
               start=200, end=340, fill=(min(255, cr + 55), min(255, cg + 55), min(255, cb + 55), 210), width=2 * os_f)
        dr.arc([cx - r_cap + os_f, cy - r_cap + os_f, cx + r_cap - os_f, cy + r_cap - os_f],
               start=20, end=160, fill=(max(0, cr - 50), max(0, cg - 50), max(0, cb - 50), 210), width=2 * os_f)

        # Crisp pointer line from center to outer skirt edge
        ir, ig, ib = indicator_rgb
        p_in = (cx + (r_cap * 0.18) * math.cos(rad), cy + (r_cap * 0.18) * math.sin(rad))
        p_out = (cx + (r_base - 2.5 * os_f) * math.cos(rad), cy + (r_base - 2.5 * os_f) * math.sin(rad))
        dr.line([p_in, p_out], fill=(12, 14, 16, 200), width=int(4.2 * os_f))
        dr.line([p_in, p_out], fill=(ir, ig, ib, 255), width=int(2.6 * os_f))

        fr = im.resize((sz, sz), Image.LANCZOS)
        strip.paste(fr, (0, fi * sz))

    strip.save(os.path.join(SKIN_DIR, filename))


def make_fader_strip(filename, fw, fh, frames, stripe_rgb):
    """Render 4x-supersampled tactile vertical ADSR slider filmstrip."""
    os_f = 4
    w, h = fw * os_f, fh * os_f
    strip = Image.new("RGBA", (fw, fh * frames), (0, 0, 0, 0))
    cx = w / 2.0
    slot_w = 5.5 * os_f
    pad_y = 14 * os_f
    travel = h - 2 * pad_y
    cap_w = fw * 0.72 * os_f
    cap_h = 18.0 * os_f

    for fi in range(frames):
        u = fi / float(max(1, frames - 1))
        ky = h - pad_y - u * travel

        im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)

        # Recessed fader track
        dr.rounded_rectangle([cx - slot_w, pad_y - 4 * os_f, cx + slot_w, h - pad_y + 4 * os_f],
                             radius=3 * os_f, fill=(10, 12, 15, 255), outline=(54, 60, 68, 255), width=os_f)
        # Active coloured fill bar under fader cap
        sr, sg, sb = stripe_rgb
        dr.rectangle([cx - 2.2 * os_f, ky, cx + 2.2 * os_f, h - pad_y + 2 * os_f], fill=(sr, sg, sb, 180))

        # Cap drop shadow
        sh = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        sdr = ImageDraw.Draw(sh)
        sdr.rounded_rectangle([cx - cap_w / 2, ky - cap_h / 2 + 3 * os_f,
                               cx + cap_w / 2, ky + cap_h / 2 + 3 * os_f],
                              radius=3 * os_f, fill=(0, 0, 0, 165))
        sh = sh.filter(ImageFilter.GaussianBlur(radius=2.2 * os_f))
        im = Image.alpha_composite(im, sh)
        dr = ImageDraw.Draw(im)

        # Sculpted charcoal Casio fader cap + glowing accent index line
        x0, y0, x1, y1 = cx - cap_w / 2, ky - cap_h / 2, cx + cap_w / 2, ky + cap_h / 2
        dr.rounded_rectangle([x0, y0, x1, y1], radius=3 * os_f,
                             fill=(46, 51, 58, 255), outline=(88, 96, 106, 255), width=os_f)
        dr.rectangle([x0 + 2 * os_f, y0 + 2 * os_f, x1 - 2 * os_f, ky - 2.5 * os_f], fill=(64, 70, 78, 255))
        dr.rectangle([x0 + 2 * os_f, ky + 2.5 * os_f, x1 - 2 * os_f, y1 - 2 * os_f], fill=(28, 32, 37, 255))
        dr.rectangle([x0 + 2 * os_f, ky - 2.2 * os_f, x1 - 2 * os_f, ky + 2.2 * os_f], fill=(sr, sg, sb, 255))

        fr = im.resize((fw, fh), Image.LANCZOS)
        strip.paste(fr, (0, fi * fh))

    strip.save(os.path.join(SKIN_DIR, filename))


def eval_cz_wave(widx, phase01, dcw=0.82):
    """Evaluate exact CrispyZebra Phase Distortion waveform at phase01 in [0, 1)."""
    local_phase = int(phase01 * 65535.0)
    dcw16 = int(dcw * 56000.0)
    rez16 = int(dcw16 * 44000 / 65536)

    def cos_inv(p16):
        return -math.cos(2.0 * math.pi * (p16 & 0xFFFF) / 65536.0)

    if widx == 0:  # SAW
        xp = 32768 + ((dcw16 * 32767) >> 16)
        if local_phase < xp:
            pd = (local_phase * 32768) // max(1, xp)
        else:
            pd = 32768 + ((local_phase - xp) * 32767) // max(1, 65535 - xp)
        return cos_inv(pd)
    elif widx == 1:  # SQUARE
        xp = 32768 - ((dcw16 * 31120) >> 16)
        if local_phase < xp:
            pd = (local_phase * 32768) // max(1, xp)
        elif local_phase < 32768:
            pd = 32768
        elif local_phase < 32768 + xp:
            pd = 32768 + ((local_phase - 32768) * 32767) // max(1, xp)
        else:
            pd = 65535
        return cos_inv(pd)
    elif widx == 2:  # PULSE
        xp = 65535 - ((dcw16 * 63487) >> 16)
        pd = (local_phase * 65535) // max(1, xp) if local_phase < xp else 65535
        return cos_inv(pd)
    elif widx == 3:  # DOUBLE SINE
        xp = 65535 - ((dcw16 * 49151) >> 16)
        if local_phase < xp:
            pd = (local_phase * 65535) // max(1, xp)
        else:
            pd = ((local_phase - xp) * 65535) // max(1, 65535 - xp)
        return cos_inv(pd)
    elif widx == 4:  # SAW-PULSE
        xp = 65535 - ((dcw16 * 32767) >> 16)
        t_pk = 32768 + ((dcw16 * 29491) >> 16)
        pk = max(1, (xp * t_pk) >> 16)
        if local_phase < xp:
            if local_phase < pk:
                pd = (local_phase * 32768) // pk
            else:
                pd = 32768 + ((local_phase - pk) * 32767) // max(1, xp - pk)
        else:
            pd = 65535
        return cos_inv(pd)
    else:  # REZ SAW (5), REZ TRI (6), REZ TRAP (7)
        res_p = (local_phase + ((local_phase * rez16 * 7) >> 16)) & 0xFFFF
        core = cos_inv(res_p) * 0.92
        if widx == 5:
            win = (65535 - local_phase) / 65535.0
        elif widx == 6:
            win = (local_phase * 2 if local_phase < 32768 else (65535 - local_phase) * 2) / 65535.0
        else:
            if local_phase < 16384:
                win = (local_phase * 4) / 65535.0
            elif local_phase > 49152:
                win = ((65535 - local_phase) * 4) / 65535.0
            else:
                win = 1.0
        return core * win


def make_wave_lcd_strip(filename, w=424, h=76):
    """Render 8-frame backlit Casio LCD Phase Distortion waveform scope filmstrip."""
    names = [
        ("01 SAWTOOTH",    "ASYMMETRIC PHASE BEND"),
        ("02 SQUARE",      "SYMMETRIC CLIPPED COSINE"),
        ("03 PULSE",       "COMPRESSED DUTY PHASE"),
        ("04 DOUBLE SINE", "OCTAVE HARMONIC FOLD"),
        ("05 SAW-PULSE",   "HALF-CYCLE RAMP + HOLD"),
        ("06 REZ SAW",     "RESONANT SAW WINDOW"),
        ("07 REZ TRI",     "RESONANT TRIANGLE WINDOW"),
        ("08 REZ TRAP",    "RESONANT TRAPEZOID WINDOW"),
    ]
    strip = Image.new("RGBA", (w, h * 8), (0, 0, 0, 0))
    for wi, (title, sub) in enumerate(names):
        im = Image.new("RGBA", (w * SS, h * SS), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)

        # Left text legend inside LCD
        draw_text(im, "WAVEFORM", 12 * SS, 14 * SS, 8.2, fill=(44, 66, 40, 220), align="left", bold=True, track=0.12)
        draw_text(im, title, 12 * SS, 35 * SS, 14.5, fill=LCD_INK, align="left", bold=True, track=0.05)
        draw_text(im, sub, 12 * SS, 57 * SS, 8.2, fill=(38, 60, 36, 230), align="left", bold=True, track=0.04)

        # Right vector scope box (2 cycles of PD waveform)
        gx0, gy0, gx1, gy1 = 198 * SS, 8 * SS, 412 * SS, 68 * SS
        dr.rectangle([gx0, gy0, gx1, gy1], outline=(56, 78, 48, 170), width=SS)
        mid_y = (gy0 + gy1) / 2.0
        dr.line([(gx0 + 2 * SS, mid_y), (gx1 - 2 * SS, mid_y)], fill=(78, 102, 68, 150), width=SS)
        mid_x = (gx0 + gx1) / 2.0
        dr.line([(mid_x, gy0 + 2 * SS), (mid_x, gy1 - 2 * SS)], fill=(78, 102, 68, 150), width=SS)

        # Plot 2 full cycles of exact CrispyZebra waveform
        pts = []
        gw = gx1 - gx0 - 8 * SS
        gh = (gy1 - gy0) * 0.40
        n_pts = 220
        for i in range(n_pts):
            t = i / float(n_pts - 1)
            ph = (t * 2.0) % 1.0
            val = eval_cz_wave(wi, ph, dcw=0.80)
            px = gx0 + 4 * SS + t * gw
            py = mid_y - val * gh
            pts.append((px, py))
        dr.line(pts, fill=LCD_INK, width=int(2.2 * SS), joint="curve")

        fr = im.resize((w, h), Image.LANCZOS)
        strip.paste(fr, (0, wi * h))

    strip.save(os.path.join(SKIN_DIR, filename))


def make_meter_lcd_strip(filename, w=26, h=62):
    """Render 8-frame backlit Casio LCD bargraph level meter (0..7 segments)."""
    strip = Image.new("RGBA", (w, h * 8), (0, 0, 0, 0))
    for lvl in range(8):
        im = Image.new("RGBA", (w * SS, h * SS), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)
        n_seg = 7
        seg_h = 5.5 * SS
        gap = 2.6 * SS
        y_bot = (h - 4) * SS
        for s in range(n_seg):
            y1 = y_bot - s * (seg_h + gap)
            y0 = y1 - seg_h
            lit = (s < lvl)
            if lit:
                col = (148, 34, 30, 255) if s == 6 else LCD_INK
            else:
                col = LCD_GHOST
            dr.rounded_rectangle([3 * SS, y0, (w - 3) * SS, y1], radius=1.5 * SS, fill=col)
        fr = im.resize((w, h), Image.LANCZOS)
        strip.paste(fr, (0, lvl * h))
    strip.save(os.path.join(SKIN_DIR, filename))


def build_page1():
    img = make_cz_chassis(seed=1984)
    draw_cz_header(img, "PHASE DISTORTION SYNTHESIZER • 8-VOICE")
    dr = ImageDraw.Draw(img)

    # 4 clean vertical module bays matching Q-Link Banks 1..4
    draw_section_card(img, 24,  126, 302, 482, "DCO / LINE 1",   CZ_TEAL,   tag="BANK 1")
    draw_section_card(img, 338, 126, 302, 482, "DCW ENVELOPE",   CZ_CORAL,  tag="BANK 2")
    draw_section_card(img, 652, 126, 302, 482, "DCA ENVELOPE",   CZ_AMBER,  tag="BANK 3")
    draw_section_card(img, 966, 126, 290, 482, "TIMBRE & OUT",   CZ_SILVER, tag="BANK 4")

    w1_short = ["SAW", "SQR", "PLS", "DSIN", "S-PL", "R-SW", "R-TR", "R-TP"]
    w2_short = ["OFF", "SAW", "SQR", "PLS", "DSIN", "S-PL", "R-SW", "R-TR", "R-TP"]

    y_top = 258
    y_bot = 468

    knobs_spec = [
        # Bank 1: DCO / LINE 1 (2x2 knobs)
        (1, "line_mode", 102, y_top, 68, "knob_cz_teal_4way.png",  4, "LINE MODE", "choice",
         choice_ticks(["L1", "L2", "1+2", "1+2'"]), -75, 75, CZ_TEAL_HI),
        (2, "l1_wave1",  248, y_top, 68, "knob_cz_teal_8way.png",  8, "L1 WAVE 1", "choice",
         choice_ticks(w1_short), -135, 135, CZ_TEAL_HI),
        (3, "l1_wave2",  102, y_bot, 68, "knob_cz_teal_9way.png",  9, "L1 WAVE 2", "choice",
         choice_ticks(w2_short), -140, 140, CZ_TEAL_HI),
        (4, "mod_mode",  248, y_bot, 68, "knob_cz_coral_4way.png", 4, "MOD MODE",  "choice",
         choice_ticks(["OFF", "RING", "NOI", "R+N"]), -75, 75, CZ_CORAL_H),

        # Bank 4: TIMBRE & OUT (2x2 knobs, DCW Depth as hero coral knob)
        (5,  "dcw_depth", 1038, y_top, 78, "knob_cz_coral_hero.png", 65, "DCW DEPTH",  "pot", None, -135, 135, CZ_CORAL_H),
        (14, "chorus",    1184, y_top, 68, "knob_cz_teal_pot.png",   65, "CHORUS",     "pot", None, -135, 135, CZ_TEAL_HI),
        (15, "drive",     1038, y_bot, 68, "knob_cz_amber_pot.png",  65, "DRIVE",      "pot", None, -135, 135, CZ_AMBER_H),
        (0,  "volume",    1184, y_bot, 74, "knob_cz_silver_hero.png",65, "MASTER VOL", "pot", None, -135, 135, IVORY),
    ]

    # Bank 2 & Bank 3: Calibrated vertical ADSR faders (DCW & DCA)
    y_fader = 368
    fh = 276
    fw = 46
    faders_spec = [
        # Bank 2: DCW ADSR (coral)
        (6,  "dcw_att", 386, y_fader, fw, fh, "fader_cz_coral.png", 65, "ATTACK",  "SLOW", "FAST", CZ_CORAL_H),
        (7,  "dcw_dec", 454, y_fader, fw, fh, "fader_cz_coral.png", 65, "DECAY",   "LONG", "SNAP", CZ_CORAL_H),
        (8,  "dcw_sus", 522, y_fader, fw, fh, "fader_cz_coral.png", 65, "SUSTAIN", "MAX",  "0",    CZ_CORAL_H),
        (13, "dcw_rel", 590, y_fader, fw, fh, "fader_cz_coral.png", 65, "RELEASE", "LONG", "DRY",  CZ_CORAL_H),

        # Bank 3: DCA ADSR (amber)
        (9,  "dca_att", 700, y_fader, fw, fh, "fader_cz_amber.png", 65, "ATTACK",  "SLOW", "FAST", CZ_AMBER_H),
        (10, "dca_dec", 768, y_fader, fw, fh, "fader_cz_amber.png", 65, "DECAY",   "LONG", "SNAP", CZ_AMBER_H),
        (11, "dca_sus", 836, y_fader, fw, fh, "fader_cz_amber.png", 65, "SUSTAIN", "MAX",  "0",    CZ_AMBER_H),
        (12, "dca_rel", 904, y_fader, fw, fh, "fader_cz_amber.png", 65, "RELEASE", "LONG", "DRY",  CZ_AMBER_H),
    ]

    knobs = []
    for pidx, kid, cx, cy, sz, strip, frames, lbl, kind, ticks, a0, a1, acc in knobs_spec:
        draw_knob_scale(img, dr, cx, cy, sz * 0.44, lbl, kind=kind, ticks=ticks,
                        start_deg=a0, end_deg=a1, accent=acc, label_y_offset=64 if sz < 74 else 68)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": sz, "h": sz,
            "strip": strip, "frames": frames, "bipolar": (kind == "bipolar"),
        })

    for pidx, kid, cx, cy, fw_, fh_, strip, frames, lbl, tlbl, blbl, acc in faders_spec:
        draw_fader_scale(img, dr, cx, cy, fw_, fh_, lbl, top_lbl=tlbl, bot_lbl=blbl, accent=acc)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": fw_, "h": fh_,
            "strip": strip, "frames": frames, "bipolar": False,
        })

    strips = [
        {"param": 2,  "id": "wave_lcd_p1", "cx": 575, "cy": 61, "w": 424, "h": 76, "strip": "wave_lcd_8.png",  "frames": 8},
        {"param": 32, "id": "meter_l_p1",  "cx": 1148, "cy": 56, "w": 26,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
        {"param": 33, "id": "meter_r_p1",  "cx": 1188, "cy": 56, "w": 26,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
    ]

    labels = [
        {"param": 34, "id": "stat_info_p1", "x": 40,  "y": 69, "w": 288, "h": 34, "size": 17, "color": "ff182a1a"},
        {"param": 35, "id": "stat_wave_p1", "x": 824, "y": 44, "w": 298, "h": 44, "size": 17, "color": "ff182a1a"},
    ]

    out = img.resize((W, H), Image.LANCZOS).convert("RGB")
    out.save(os.path.join(SKIN_DIR, "bg_czboy.png"))
    return {"knobs": knobs, "strips": strips, "labels": labels}


def build_page2():
    img = make_cz_chassis(seed=1985)
    draw_cz_header(img, "LINE 2 DETUNE • PITCH EG • VIBRATO • VOICE")
    dr = ImageDraw.Draw(img)

    draw_section_card(img, 24,  126, 302, 482, "LINE 2 WAVE",       CZ_TEAL,   tag="BANK 1")
    draw_section_card(img, 338, 126, 302, 482, "DETUNE & PITCH EG", CZ_CORAL,  tag="BANK 2")
    draw_section_card(img, 652, 126, 302, 482, "VIBRATO LFO",       CZ_AMBER,  tag="BANK 3")
    draw_section_card(img, 966, 126, 290, 482, "VOICE & ENGINE",    CZ_SILVER, tag="BANK 4")

    w1_short = ["SAW", "SQR", "PLS", "DSIN", "S-PL", "R-SW", "R-TR", "R-TP"]
    w2_short = ["OFF", "SAW", "SQR", "PLS", "DSIN", "S-PL", "R-SW", "R-TR", "R-TP"]

    y_top = 258
    y_bot = 468

    knobs_spec = [
        # Bank 1: LINE 2 WAVE
        (16, "l2_wave1", 102, y_top, 68, "knob_cz_teal_8way.png",  8,  "L2 WAVE 1", "choice",
         choice_ticks(w1_short), -135, 135, CZ_TEAL_HI),
        (17, "l2_wave2", 248, y_top, 68, "knob_cz_teal_9way.png",  9,  "L2 WAVE 2", "choice",
         choice_ticks(w2_short), -140, 140, CZ_TEAL_HI),
        (18, "l2_dcw",   102, y_bot, 68, "knob_cz_coral_pot.png",  65, "L2 DCW",    "pot", None, -135, 135, CZ_CORAL_H),
        (19, "l2_oct",   248, y_bot, 68, "knob_cz_teal_5way.png",  5,  "L2 OCTAVE", "choice",
         choice_ticks(["-2", "-1", "0", "+1", "+2"]), -100, 100, CZ_TEAL_HI),

        # Bank 2: DETUNE & PITCH EG
        (20, "l2_semi",   416, y_top, 68, "knob_cz_silver_pot.png", 65, "L2 SEMI",   "bipolar", None, -135, 135, CZ_CORAL_H),
        (21, "l2_fine",   562, y_top, 68, "knob_cz_silver_pot.png", 65, "L2 FINE",   "bipolar", None, -135, 135, CZ_CORAL_H),
        (22, "pitch_env", 416, y_bot, 68, "knob_cz_coral_pot.png",  65, "PITCH ENV", "pot",     None, -135, 135, CZ_CORAL_H),
        (23, "pitch_dec", 562, y_bot, 68, "knob_cz_coral_pot.png",  65, "PITCH DEC", "pot",     None, -135, 135, CZ_CORAL_H),

        # Bank 3: VIBRATO LFO
        (24, "vib_wave",  730, y_top, 68, "knob_cz_amber_4way.png", 4,  "VIB WAVE",  "choice",
         choice_ticks(["TRI", "SQR", "UP", "DN"]), -75, 75, CZ_AMBER_H),
        (25, "vib_rate",  876, y_top, 68, "knob_cz_amber_pot.png",  65, "VIB RATE",  "pot", None, -135, 135, CZ_AMBER_H),
        (26, "vib_depth", 730, y_bot, 68, "knob_cz_amber_pot.png",  65, "VIB DEPTH", "pot", None, -135, 135, CZ_AMBER_H),
        (27, "vib_delay", 876, y_bot, 68, "knob_cz_amber_pot.png",  65, "VIB DELAY", "pot", None, -135, 135, CZ_AMBER_H),

        # Bank 4: VOICE & ENGINE
        (28, "voice_mode", 1038, y_top, 68, "knob_cz_teal_3way.png",   3,  "VOICE MODE", "choice",
         choice_ticks(["POLY8", "POLY4", "MONO"]), -60, 60, CZ_TEAL_HI),
        (29, "glide",      1184, y_top, 68, "knob_cz_silver_pot.png",  65, "GLIDE",      "pot", None, -135, 135, IVORY),
        (30, "vel_sens",   1038, y_bot, 68, "knob_cz_silver_pot.png",  65, "VELOCITY",   "pot", None, -135, 135, IVORY),
        (31, "dca_curve",  1184, y_bot, 68, "knob_cz_amber_2way.png",  2,  "DCA CURVE",  "choice",
         choice_ticks(["LIN", "EXP"]), -40, 40, CZ_AMBER_H),
    ]

    knobs = []
    for pidx, kid, cx, cy, sz, strip, frames, lbl, kind, ticks, a0, a1, acc in knobs_spec:
        draw_knob_scale(img, dr, cx, cy, sz * 0.44, lbl, kind=kind, ticks=ticks,
                        start_deg=a0, end_deg=a1, accent=acc, label_y_offset=64)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": sz, "h": sz,
            "strip": strip, "frames": frames, "bipolar": (kind == "bipolar"),
        })

    strips = [
        {"param": 16, "id": "wave_lcd_p2", "cx": 575,  "cy": 61, "w": 424, "h": 76, "strip": "wave_lcd_8.png",  "frames": 8},
        {"param": 32, "id": "meter_l_p2",  "cx": 1148, "cy": 56, "w": 26,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
        {"param": 33, "id": "meter_r_p2",  "cx": 1188, "cy": 56, "w": 26,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
    ]

    labels = [
        {"param": 34, "id": "stat_info_p2", "x": 40,  "y": 69, "w": 288, "h": 34, "size": 17, "color": "ff182a1a"},
        {"param": 35, "id": "stat_wave_p2", "x": 824, "y": 44, "w": 298, "h": 44, "size": 17, "color": "ff182a1a"},
    ]

    out = img.resize((W, H), Image.LANCZOS).convert("RGB")
    out.save(os.path.join(SKIN_DIR, "bg_line2.png"))
    return {"knobs": knobs, "strips": strips, "labels": labels}


def main():
    os.makedirs(SKIN_DIR, exist_ok=True)
    print("Rendering CZboy Page 1 (1984 Casio CZ Console + Live PD LCD + ADSR Faders)...")
    p1 = build_page1()
    print("Rendering CZboy Page 2 (Line 2 Detune / Pitch EG / Vibrato / Voice)...")
    p2 = build_page2()

    print("Rendering 4x-supersampled Casio CZ Knobs, ADSR Faders & LCD Strips...")
    make_wave_lcd_strip("wave_lcd_8.png", 424, 76)
    make_meter_lcd_strip("meter_lcd_8.png", 26, 62)

    # Knobs
    make_knob_strip("knob_cz_teal_pot.png",    68, 65, (38, 176, 176))
    make_knob_strip("knob_cz_teal_3way.png",   68, 3,  (38, 176, 176), -60, 60)
    make_knob_strip("knob_cz_teal_4way.png",   68, 4,  (38, 176, 176), -75, 75)
    make_knob_strip("knob_cz_teal_5way.png",   68, 5,  (38, 176, 176), -100, 100)
    make_knob_strip("knob_cz_teal_8way.png",   68, 8,  (38, 176, 176), -135, 135)
    make_knob_strip("knob_cz_teal_9way.png",   68, 9,  (38, 176, 176), -140, 140)

    make_knob_strip("knob_cz_coral_hero.png",  78, 65, (220, 74, 68))
    make_knob_strip("knob_cz_coral_pot.png",   68, 65, (220, 74, 68))
    make_knob_strip("knob_cz_coral_4way.png",  68, 4,  (220, 74, 68), -75, 75)

    make_knob_strip("knob_cz_amber_pot.png",   68, 65, (224, 166, 48))
    make_knob_strip("knob_cz_amber_2way.png",  68, 2,  (224, 166, 48), -40, 40)
    make_knob_strip("knob_cz_amber_4way.png",  68, 4,  (224, 166, 48), -75, 75)

    make_knob_strip("knob_cz_silver_hero.png", 74, 65, (186, 196, 204), indicator_rgb=(220, 66, 58))
    make_knob_strip("knob_cz_silver_pot.png",  68, 65, (172, 182, 190), indicator_rgb=(28, 32, 36))

    # Faders
    make_fader_strip("fader_cz_coral.png", 46, 276, 65, (236, 94, 84))
    make_fader_strip("fader_cz_amber.png", 46, 276, 65, (238, 184, 66))

    with open(MANIFEST, "w", encoding="utf-8") as fh:
        json.dump({"page1": p1, "page2": p2}, fh, indent=2)
    print(f"Done! Wrote CZboy hardware skins to {SKIN_DIR}")


if __name__ == "__main__":
    main()
