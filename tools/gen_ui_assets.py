#!/usr/bin/env python3
"""Procedural 1984 Casio CZ-101 Hardware Faceplate UI generator for CZboy.

Aesthetic:
  - Matte jet-charcoal Casio molded ABS deck with fine satin texture & recessed case bezel
  - Authentic 1984 Casio CZ-101 hairline technical schematic grid (DCO -> DCW -> DCA)
    with colour-coded flat Casio section pills (Aqua-Teal, Signal Coral-Orange, Warm Ivory, Silver)
  - Brushed-silver chamfered bezel framing a recessed backlit green-grey Casio LCD
    (2-line readout + live 8-waveform Phase Distortion vector scope + slim LCD meters)
  - Authentic CZ-101 printed 8-waveform reference legend right on the faceplate
  - Flat matte-black cylindrical CZ-style rotary dials (Row 1) + compact sculpted
    Casio CZ-101 mini-sliders with 0..99 hairline scales (Row 2)
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

# 1984 Casio CZ-101 Industrial Palette
WHITE      = (238, 238, 234, 255)
OFF_WHITE  = (184, 188, 190, 255)
MUTED      = (118, 124, 130, 255)
DARK_INK   = (18, 22, 24, 255)

CZ_TEAL    = (30, 174, 180, 255)
CZ_TEAL_HI = (72, 212, 216, 255)
CZ_CORAL   = (224, 82, 58, 255)
CZ_CORAL_H = (246, 114, 86, 255)
CZ_IVORY   = (222, 214, 188, 255)
CZ_SILVER  = (158, 166, 174, 255)
CZ_LINE    = (92, 98, 106, 255)

# Authentic Casio CZ-101 Silver-Framed Green-Grey LCD
LCD_BG     = (146, 170, 120, 255)
LCD_INK    = (22, 36, 24, 255)
LCD_DIM    = (54, 74, 48, 235)
LCD_GHOST  = (124, 148, 102, 255)


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


def draw_text(img, text, x, y, size_px, fill=WHITE, align="center", bold=True, track=0.05):
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


def eval_cz_wave(widx, phase01, dcw=0.86):
    """Evaluate exact CrispyZebra Phase Distortion waveform at phase01 in [0, 1)."""
    local_phase = int(phase01 * 65535.0)
    dcw16 = int(dcw * 64200.0)
    rez16 = int(dcw * 56000.0)

    def cos_inv(p16):
        return -math.cos(2.0 * math.pi * (p16 & 0xFFFF) / 65536.0)

    if widx == 0:  # SAW
        xp = 32768 + ((dcw16 * 32767) >> 16)
        pd = (local_phase * 32768) // max(1, xp) if local_phase < xp else 32768 + ((local_phase - xp) * 32767) // max(1, 65535 - xp)
        return cos_inv(pd)
    elif widx == 1:  # SQUARE
        xp = max(64, 32768 - ((dcw16 * 31600) >> 16))
        if local_phase < xp:
            pd = (local_phase * 32768) // xp
        elif local_phase < 32768:
            pd = 32768
        elif local_phase < 32768 + xp:
            pd = 32768 + ((local_phase - 32768) * 32767) // xp
        else:
            pd = 65535
        return cos_inv(pd)
    elif widx == 2:  # PULSE
        xp = max(128, 65535 - ((dcw16 * 63800) >> 16))
        pd = (local_phase * 65535) // xp if local_phase < xp else 65535
        return cos_inv(pd)
    elif widx == 3:  # DOUBLE SINE
        xp = 65535 - ((dcw16 * 54000) >> 16)
        if local_phase < xp:
            pd = (local_phase * 65535) // max(1, xp)
        else:
            pd = ((local_phase - xp) * 65535) // max(1, 65535 - xp)
        return cos_inv(pd)
    elif widx == 4:  # SAW-PULSE
        xp = 65535 - ((dcw16 * 32767) >> 16)
        t_pk = 32768 + ((dcw16 * 30500) >> 16)
        pk = max(1, (xp * t_pk) >> 16)
        if local_phase < xp:
            pd = (local_phase * 32768) // pk if local_phase < pk else 32768 + ((local_phase - pk) * 32767) // max(1, xp - pk)
        else:
            pd = 65535
        return cos_inv(pd)
    else:  # REZ SAW (5), REZ TRI (6), REZ TRAP (7) — real Casio unipolar (1-cos)*window
        res_p = (local_phase + ((local_phase * rez16 * 10) >> 16)) & 0xFFFF
        core_uni = (1.0 + cos_inv(res_p)) * 0.5
        if widx == 5:
            win = (65535 - local_phase) / 65535.0
            return max(-1.0, min(1.0, (core_uni * win - 0.25) * 1.9))
        elif widx == 6:
            win = (local_phase * 2 if local_phase < 32768 else (65535 - local_phase) * 2) / 65535.0
            return max(-1.0, min(1.0, (core_uni * win - 0.25) * 1.9))
        else:
            if local_phase < 16384:
                win = (local_phase * 4) / 65535.0
            elif local_phase > 49152:
                win = ((65535 - local_phase) * 4) / 65535.0
            else:
                win = 1.0
            return max(-1.0, min(1.0, (core_uni * win - 0.375) * 1.65))


def make_cz101_chassis(seed=101):
    """Authentic 1984 Casio CZ-101 flat matte jet-charcoal molded ABS console body."""
    w, h = W * SS, H * SS
    rng = random.Random(seed)
    base = Image.new("RGBA", (w, h), (26, 29, 33, 255))
    dr = ImageDraw.Draw(base)

    # Subtle matte ABS top-edge studio falloff
    for y in range(h):
        t = y / float(h)
        d = int(round(5.0 * (1.0 - t * 1.15)))
        c = (max(18, 25 + d), max(21, 28 + d), max(24, 32 + d), 255)
        dr.line([(0, y), (w, y)], fill=c)

    # Fine Casio molded ABS micro-grain
    grain = Image.new("RGBA", (w // 2, h // 2), (0, 0, 0, 0))
    px = grain.load()
    for gy in range(h // 2):
        for gx in range(w // 2):
            v = rng.randint(-4, 4)
            if v > 0:
                px[gx, gy] = (230, 234, 238, v * 2)
            elif v < 0:
                px[gx, gy] = (4, 6, 8, -v * 2)
    grain = grain.resize((w, h), Image.NEAREST)
    base = Image.alpha_composite(base, grain)
    dr = ImageDraw.Draw(base)

    # Recessed matte outer casing bevel (authentic CZ-101 clean black plastic frame)
    dr.rectangle([0, 0, w - 1, 5 * SS], fill=(42, 46, 52, 255))
    dr.rectangle([0, h - 5 * SS, w - 1, h - 1], fill=(14, 16, 18, 255))
    dr.rectangle([0, 0, 5 * SS, h - 1], fill=(34, 38, 43, 255))
    dr.rectangle([w - 5 * SS, 0, w - 1, h - 1], fill=(16, 18, 21, 255))

    # Crisp Casio CZ-101 top fascia separator rules (signature white + coral-orange + teal thin lines)
    dr.rectangle([10 * SS, 8 * SS, w - 10 * SS, 118 * SS], fill=(21, 24, 28, 255))
    dr.line([(10 * SS, 118 * SS), (w - 10 * SS, 118 * SS)], fill=(145, 152, 160, 220), width=SS)
    dr.line([(10 * SS, 121 * SS), (w - 10 * SS, 121 * SS)], fill=(224, 82, 58, 210), width=int(1.5 * SS))
    dr.line([(10 * SS, 124 * SS), (w - 10 * SS, 124 * SS)], fill=(30, 174, 180, 190), width=SS)

    return base


def draw_silver_cz_lcd(img, x, y, w, h):
    """Authentic 1984 Casio CZ-101 brushed-silver plastic bezel + green-grey LCD."""
    dr = ImageDraw.Draw(img)
    x0, y0, x1, y1 = int(x * SS), int(y * SS), int((x + w) * SS), int((y + h) * SS)

    # Outer recessed black shadow slot
    dr.rounded_rectangle([x0 - 6 * SS, y0 - 6 * SS, x1 + 6 * SS, y1 + 6 * SS],
                         radius=5 * SS, fill=(10, 12, 14, 255))
    # Brushed-silver chamfered Casio LCD bezel
    dr.rounded_rectangle([x0 - 4 * SS, y0 - 4 * SS, x1 + 4 * SS, y1 + 4 * SS],
                         radius=4 * SS, fill=(168, 174, 180, 255), outline=(210, 216, 222, 255), width=SS)
    dr.rounded_rectangle([x0 - 2 * SS, y0 - 2 * SS, x1 + 2 * SS, y1 + 2 * SS],
                         radius=3 * SS, fill=(96, 102, 108, 255))

    # Green-grey LCD glass surface
    lw, lh = x1 - x0, y1 - y0
    lcd = Image.new("RGBA", (lw, lh), LCD_BG)
    ldr = ImageDraw.Draw(lcd)
    for yy in range(lh):
        t = yy / float(max(1, lh - 1))
        d = int(round(9 * (1.0 - abs(t - 0.30) * 1.45)))
        ldr.line([(0, yy), (lw, yy)], fill=(
            max(100, min(180, LCD_BG[0] + d)),
            max(120, min(200, LCD_BG[1] + d)),
            max(80,  min(150, LCD_BG[2] + d)),
            255,
        ))
    # Inner bezel glass shadow
    ldr.rectangle([0, 0, lw - 1, 3 * SS], fill=(72, 92, 62, 150))
    ldr.rectangle([0, 0, 3 * SS, lh - 1], fill=(72, 92, 62, 120))
    # Crisp 2-line Casio LCD separator on left readout half + vertical divider before scope
    div_x = 266 * SS
    ldr.line([(10 * SS, lh // 2), (div_x - 10 * SS, lh // 2)], fill=(96, 118, 82, 150), width=SS)
    ldr.line([(div_x, 6 * SS), (div_x, lh - 6 * SS)], fill=(78, 100, 66, 180), width=SS)
    img.alpha_composite(lcd, (x0, y0))

    dr = ImageDraw.Draw(img)
    dr.rectangle([x0, y0, x1, y1], outline=(28, 38, 28, 220), width=SS)


def draw_waveform_legend_chart(img, x, y, w, h):
    """Authentic Casio CZ-101 printed 8-waveform silkscreen reference chart on the faceplate."""
    dr = ImageDraw.Draw(img)
    x0, y0, x1, y1 = int(x * SS), int(y * SS), int((x + w) * SS), int((y + h) * SS)
    dr.rounded_rectangle([x0, y0, x1, y1], radius=3 * SS, fill=(17, 20, 24, 255), outline=CZ_LINE, width=SS)
    draw_text(img, "CZ WAVEFORM TABLE", (x + w / 2) * SS, (y + 10) * SS, 7.6, fill=OFF_WHITE, align="center", bold=True, track=0.14)
    dr.line([(x0 + 6 * SS, (y + 19) * SS), (x1 - 6 * SS, (y + 19) * SS)], fill=(64, 70, 78, 255), width=SS)

    labels = ["1 SAW", "2 SQR", "3 PLS", "4 DBL", "5 S-PL", "6 R-SW", "7 R-TR", "8 R-TP"]
    cw = (w - 14) / 4.0
    ch = (h - 24) / 2.0
    for idx, lbl in enumerate(labels):
        col = idx % 4
        row = idx // 4
        cx = x + 7 + col * cw
        cy = y + 23 + row * ch
        # Draw tiny crisp silkscreen waveform glyph
        gx0 = (cx + 6) * SS
        gx1 = (cx + cw - 6) * SS
        gy_mid = (cy + 13) * SS
        gh = 8.0 * SS
        pts = []
        for s in range(36):
            t = s / 35.0
            v = eval_cz_wave(idx, t, dcw=0.80)
            pts.append((gx0 + t * (gx1 - gx0), gy_mid - v * gh))
        line_col = CZ_CORAL_H if idx >= 5 else CZ_TEAL_HI
        dr.line(pts, fill=line_col, width=int(1.25 * SS))
        draw_text(img, lbl, (cx + cw / 2) * SS, (cy + 27) * SS, 7.0, fill=OFF_WHITE, align="center", bold=True, track=0.04)


def draw_cz101_header(img, page_tag):
    """Draw the iconic 1984 Casio CZ-101 header: nameplate + silver LCD + waveform legend."""
    dr = ImageDraw.Draw(img)

    # Left: Bold flat 1984 Casio-style CZboy logotype
    draw_text(img, "CZ", 28 * SS, 36 * SS, 33, fill=WHITE, align="left", bold=True, track=0.01)
    draw_text(img, "boy", 79 * SS, 38 * SS, 30, fill=CZ_TEAL_HI, align="left", bold=True, track=0.01)

    # Crisp Casio model pill badge next to logo
    dr.rounded_rectangle([168 * SS, 22 * SS, 232 * SS, 42 * SS], radius=2 * SS, fill=CZ_CORAL)
    draw_text(img, "PD-101", 200 * SS, 32.5 * SS, 9.2, fill=WHITE, align="center", bold=True, track=0.08)

    # Thin double Casio stripe under logo
    dr.rectangle([28 * SS, 57 * SS, 316 * SS, 60 * SS], fill=CZ_TEAL)
    dr.rectangle([28 * SS, 62 * SS, 224 * SS, 64 * SS], fill=CZ_CORAL)
    draw_text(img, "PHASE DISTORTION SYNTHESIZER", 28 * SS, 76 * SS, 9.2, fill=WHITE, align="left", bold=True, track=0.11)
    draw_text(img, page_tag, 28 * SS, 95 * SS, 8.2, fill=OFF_WHITE, align="left", bold=True, track=0.08)

    # Center: Silver-framed Casio LCD (x=344..882, y=18..106)
    draw_silver_cz_lcd(img, 344, 18, 538, 88)
    draw_text(img, "L", 834 * SS, 98 * SS, 7.6, fill=LCD_INK, align="center", bold=True)
    draw_text(img, "R", 862 * SS, 98 * SS, 7.6, fill=LCD_INK, align="center", bold=True)

    # Right: Authentic CZ-101 Waveform Table printed on faceplate (x=904..1254, y=16..108)
    draw_waveform_legend_chart(img, 904, 16, 350, 92)


def draw_cz_schematic_box(img, x, y, w, h, badge_text, sub_text, accent, bank_tag):
    """Draw a true 1984 Casio CZ-101 hairline schematic outline box with flat colour pill header."""
    dr = ImageDraw.Draw(img)
    x0, y0, x1, y1 = int(x * SS), int(y * SS), int((x + w) * SS), int((y + h) * SS)

    # Slightly darker recessed inner work area + crisp hairline Casio outline
    dr.rounded_rectangle([x0, y0 + 10 * SS, x1, y1], radius=4 * SS,
                         fill=(20, 23, 27, 255), outline=CZ_LINE, width=int(1.25 * SS))
    # Subtle accent hairline along top edge inside box
    dr.line([(x0 + 4 * SS, y0 + 10 * SS), (x1 - 4 * SS, y0 + 10 * SS)], fill=accent, width=int(1.5 * SS))

    # Flat rectangular Casio silkscreen colour badge straddling the top border
    bw = (len(badge_text) * 9.6 + 24) * SS
    bx0 = x0 + 14 * SS
    by0 = y0
    by1 = y0 + 21 * SS
    dr.rectangle([bx0, by0, bx0 + bw, by1], fill=accent)
    ink = DARK_INK if (accent[0] * 0.3 + accent[1] * 0.59 + accent[2] * 0.11) > 130 else WHITE
    draw_text(img, badge_text, bx0 + bw / 2.0, (by0 + by1) / 2.0, 10.6, fill=ink, align="center", bold=True, track=0.10)

    # Clean technical sub-label inline on top border with matte-charcoal knock-out gap
    sub_x = bx0 + bw + 8 * SS
    sub_w = (len(sub_text) * 6.4 + 14) * SS
    dr.rectangle([sub_x - 2 * SS, by0 + 2 * SS, sub_x + sub_w, by1 - 2 * SS], fill=(24, 27, 31, 255))
    draw_text(img, sub_text, sub_x + 5 * SS, (by0 + by1) / 2.0 + 0.5 * SS, 8.6, fill=OFF_WHITE, align="left", bold=True, track=0.08)

    # Subtle Q-Link bank indicator on right of top line
    tbw = 58 * SS
    tx1 = x1 - 12 * SS
    dr.rectangle([tx1 - tbw, by0 + 2 * SS, tx1, by1 - 2 * SS], fill=(26, 29, 33, 255), outline=CZ_LINE, width=SS)
    draw_text(img, bank_tag, tx1 - tbw / 2.0, (by0 + by1) / 2.0, 7.6, fill=MUTED, align="center", bold=True, track=0.08)


def draw_signal_arrow(img, x0, x1, y, accent=CZ_TEAL_HI):
    """Draw a crisp Casio schematic signal-flow arrow connecting blocks."""
    dr = ImageDraw.Draw(img)
    yy = int(y * SS)
    xa, xb = int(x0 * SS), int(x1 * SS)
    dr.line([(xa, yy), (xb - 6 * SS, yy)], fill=accent, width=int(2.0 * SS))
    dr.polygon([(xb, yy), (xb - 9 * SS, yy - 5 * SS), (xb - 9 * SS, yy + 5 * SS)], fill=accent)


def choice_ticks(labels):
    n = len(labels)
    if n <= 1:
        return [(0.0, labels[0])]
    return [(i / float(n - 1), lb) for i, lb in enumerate(labels)]


def draw_dial_scale(img, dr, cx, cy, r_dial, label, kind="pot", ticks=None,
                    start_deg=-135, end_deg=135, accent=CZ_TEAL_HI, label_y_offset=61):
    cx_s, cy_s = cx * SS, cy * SS
    r_in = (r_dial + 5.0) * SS
    r_out = (r_dial + 10.5) * SS
    r_lbl = (r_dial + 21.0) * SS

    def pt(deg, rad):
        a = math.radians(deg - 90.0)
        return (cx_s + rad * math.cos(a), cy_s + rad * math.sin(a))

    if kind == "choice" and ticks:
        for norm, txt in ticks:
            deg = start_deg + norm * (end_deg - start_deg)
            dr.line([pt(deg, r_in), pt(deg, r_out + 1.2 * SS)], fill=accent, width=int(1.6 * SS))
            lx, ly = pt(deg, r_lbl + 2 * SS)
            draw_text(img, txt, lx, ly, 8.0, fill=WHITE, align="center", bold=True, track=0.02)
    else:
        for i in range(11):
            u = i / 10.0
            deg = start_deg + u * (end_deg - start_deg)
            major = (i in (0, 5, 10))
            col = accent if (kind == "bipolar" and i == 5) else (OFF_WHITE if major else MUTED)
            dr.line([pt(deg, r_in), pt(deg, r_out if major else r_out - 2.5 * SS)],
                    fill=col, width=int((1.6 if major else 1.0) * SS))
        if kind == "bipolar":
            draw_text(img, "-", *pt(start_deg, r_lbl), 9.5, fill=OFF_WHITE, align="center", bold=True)
            draw_text(img, "0", *pt(0, r_lbl - 2 * SS), 8.2, fill=accent, align="center", bold=True)
            draw_text(img, "+", *pt(end_deg, r_lbl), 9.5, fill=OFF_WHITE, align="center", bold=True)
        else:
            draw_text(img, "0",  *pt(start_deg, r_lbl), 8.0, fill=OFF_WHITE, align="center", bold=True)
            draw_text(img, "50", *pt(0, r_lbl - 2 * SS), 7.6, fill=MUTED, align="center", bold=True)
            draw_text(img, "99", *pt(end_deg, r_lbl), 8.0, fill=OFF_WHITE, align="center", bold=True)

    draw_text(img, label, cx_s, (cy + label_y_offset) * SS, 10.2, fill=WHITE, align="center", bold=True, track=0.08)


def draw_mini_slider_scale(img, dr, cx, cy, fw, fh, label, top_lbl="99", bot_lbl="0",
                           bipolar=False, accent=CZ_CORAL_H):
    """Authentic 1984 Casio CZ-101 mini-slider 0..99 hairline graduation scale."""
    cx_s, cy_s = cx * SS, cy * SS
    half_h = (fh * 0.39) * SS
    xl = (cx - fw * 0.48) * SS
    xr = (cx + fw * 0.48) * SS

    for i in range(9):
        u = i / 8.0
        yy = cy_s + half_h - u * (2.0 * half_h)
        major = (i in (0, 4, 8))
        tlen = (9.0 if major else 5.0) * SS
        col = accent if ((bipolar and i == 4) or (not bipolar and i == 8)) else (OFF_WHITE if major else MUTED)
        w_ln = int((1.5 if major else 1.0) * SS)
        dr.line([(xl - tlen, yy), (xl - 2 * SS, yy)], fill=col, width=w_ln)
        dr.line([(xr + 2 * SS, yy), (xr + tlen, yy)], fill=col, width=w_ln)

    draw_text(img, top_lbl, cx_s, (cy - fh * 0.50 - 9) * SS, 8.0, fill=accent, align="center", bold=True, track=0.04)
    draw_text(img, bot_lbl, cx_s, (cy + fh * 0.50 + 8) * SS, 8.0, fill=MUTED, align="center", bold=True, track=0.04)
    draw_text(img, label,   cx_s, (cy + fh * 0.50 + 25) * SS, 10.2, fill=WHITE, align="center", bold=True, track=0.08)


def make_flat_cz_dial(filename, sz, frames, indicator_rgb, start_deg=-135, end_deg=135):
    """Render 4x-supersampled flat matte-black Casio CZ-style cylindrical rotary dial."""
    os_f = 4
    s = sz * os_f
    strip = Image.new("RGBA", (sz, sz * frames), (0, 0, 0, 0))
    cx = cy = s / 2.0
    r_outer = s * 0.43
    r_top   = s * 0.36

    for fi in range(frames):
        u = fi / float(max(1, frames - 1))
        deg = start_deg + u * (end_deg - start_deg)
        rad = math.radians(deg - 90.0)

        im = Image.new("RGBA", (s, s), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)

        # Clean low-profile contact shadow
        sh = Image.new("RGBA", (s, s), (0, 0, 0, 0))
        sdr = ImageDraw.Draw(sh)
        sdr.ellipse([cx - r_outer, cy - r_outer + 3.5 * os_f, cx + r_outer, cy + r_outer + 3.5 * os_f],
                    fill=(0, 0, 0, 155))
        sh = sh.filter(ImageFilter.GaussianBlur(radius=2.2 * os_f))
        im = Image.alpha_composite(im, sh)
        dr = ImageDraw.Draw(im)

        # Matte-black cylindrical skirt + subtle precision rim
        dr.ellipse([cx - r_outer, cy - r_outer, cx + r_outer, cy + r_outer],
                   fill=(24, 27, 31, 255), outline=(66, 72, 80, 255), width=int(1.6 * os_f))

        # Ultra-fine Casio micro-knurling ticks around bevel
        for fl in range(36):
            a = rad + fl * (2.0 * math.pi / 36.0)
            p0 = (cx + (r_top + 1 * os_f) * math.cos(a), cy + (r_top + 1 * os_f) * math.sin(a))
            p1 = (cx + (r_outer - 1.5 * os_f) * math.cos(a), cy + (r_outer - 1.5 * os_f) * math.sin(a))
            dr.line([p0, p1], fill=(42, 46, 52, 190), width=os_f)

        # Flat matte-black machined top disc (completely flat, no dome cap!)
        dr.ellipse([cx - r_top, cy - r_top, cx + r_top, cy + r_top],
                   fill=(32, 36, 41, 255), outline=(56, 62, 68, 255), width=os_f)
        # Subtle top chamfer light arc
        dr.arc([cx - r_top + os_f, cy - r_top + os_f, cx + r_top - os_f, cy + r_top - os_f],
               start=210, end=330, fill=(84, 92, 100, 140), width=int(1.4 * os_f))
        # Subtle recessed inner lathe ring
        r_mid = r_top * 0.62
        dr.ellipse([cx - r_mid, cy - r_mid, cx + r_mid, cy + r_mid],
                   outline=(22, 25, 29, 180), width=os_f)

        # Crisp recessed coloured Casio pointer bar flush with the flat top
        ir, ig, ib = indicator_rgb
        p_in  = (cx + (r_top * 0.24) * math.cos(rad), cy + (r_top * 0.24) * math.sin(rad))
        p_out = (cx + (r_top - 1.5 * os_f) * math.cos(rad), cy + (r_top - 1.5 * os_f) * math.sin(rad))
        dr.line([p_in, p_out], fill=(10, 12, 14, 220), width=int(5.0 * os_f))
        dr.line([p_in, p_out], fill=(ir, ig, ib, 255), width=int(3.2 * os_f))

        fr = im.resize((sz, sz), Image.LANCZOS)
        strip.paste(fr, (0, fi * sz))

    strip.save(os.path.join(SKIN_DIR, filename))


def make_cz_mini_slider(filename, fw, fh, frames, stripe_rgb):
    """Render 4x-supersampled compact 1984 Casio CZ-101 vertical mini-slider filmstrip."""
    os_f = 4
    w, h = fw * os_f, fh * os_f
    strip = Image.new("RGBA", (fw, fh * frames), (0, 0, 0, 0))
    cx = w / 2.0
    slot_w = 4.2 * os_f
    pad_y = 14 * os_f
    travel = h - 2 * pad_y
    cap_w = fw * 0.78 * os_f
    cap_h = 20.0 * os_f

    sr, sg, sb = stripe_rgb

    for fi in range(frames):
        u = fi / float(max(1, frames - 1))
        ky = h - pad_y - u * travel

        im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)

        # Recessed Casio rectangular channel housing
        dr.rectangle([cx - slot_w - 3 * os_f, pad_y - 6 * os_f, cx + slot_w + 3 * os_f, h - pad_y + 6 * os_f],
                     fill=(13, 15, 18, 255), outline=(52, 57, 64, 255), width=os_f)
        # Inner slider slot
        dr.rectangle([cx - 2.2 * os_f, pad_y - 2 * os_f, cx + 2.2 * os_f, h - pad_y + 2 * os_f],
                     fill=(6, 7, 9, 255))
        # Subtle active level line inside slot
        dr.rectangle([cx - 1.6 * os_f, ky, cx + 1.6 * os_f, h - pad_y + 1 * os_f], fill=(sr, sg, sb, 165))

        # Crisp shadow behind compact rectangular Casio slider thumb
        sh = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        sdr = ImageDraw.Draw(sh)
        sdr.rectangle([cx - cap_w / 2, ky - cap_h / 2 + 2.5 * os_f,
                       cx + cap_w / 2, ky + cap_h / 2 + 2.5 * os_f], fill=(0, 0, 0, 175))
        sh = sh.filter(ImageFilter.GaussianBlur(radius=1.8 * os_f))
        im = Image.alpha_composite(im, sh)
        dr = ImageDraw.Draw(im)

        # Authentic flat matte-black Casio CZ-101 rectangular slider thumb with grip ridges
        x0, y0, x1, y1 = cx - cap_w / 2, ky - cap_h / 2, cx + cap_w / 2, ky + cap_h / 2
        dr.rounded_rectangle([x0, y0, x1, y1], radius=2 * os_f,
                             fill=(34, 38, 43, 255), outline=(78, 84, 92, 255), width=os_f)
        # Top/bottom moulded bevel ridges (classic Casio slider feel)
        dr.rectangle([x0 + 2 * os_f, y0 + 2 * os_f, x1 - 2 * os_f, y0 + 5.2 * os_f], fill=(56, 62, 70, 255))
        dr.rectangle([x0 + 2 * os_f, y1 - 5.2 * os_f, x1 - 2 * os_f, y1 - 2 * os_f], fill=(22, 25, 29, 255))
        # Razor-sharp recessed coloured Casio centre index bar
        dr.rectangle([x0 + 2 * os_f, ky - 2.0 * os_f, x1 - 2 * os_f, ky + 2.0 * os_f], fill=(sr, sg, sb, 255))

        fr = im.resize((fw, fh), Image.LANCZOS)
        strip.paste(fr, (0, fi * fh))

    strip.save(os.path.join(SKIN_DIR, filename))


def make_wave_lcd_strip(filename, w=200, h=76):
    """Render compact 8-frame Casio LCD Phase Distortion waveform scope inside the silver LCD."""
    names = ["01 SAW", "02 SQR", "03 PLS", "04 DBL", "05 S-PL", "06 RZ-SW", "07 RZ-TR", "08 RZ-TP"]
    strip = Image.new("RGBA", (w, h * 8), (0, 0, 0, 0))
    for wi, title in enumerate(names):
        im = Image.new("RGBA", (w * SS, h * SS), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)

        draw_text(im, title, 8 * SS, 11 * SS, 8.8, fill=LCD_INK, align="left", bold=True, track=0.06)
        gx0, gy0, gx1, gy1 = 6 * SS, 22 * SS, (w - 6) * SS, (h - 6) * SS
        dr.rectangle([gx0, gy0, gx1, gy1], outline=(76, 98, 64, 175), width=SS)
        mid_y = (gy0 + gy1) / 2.0
        dr.line([(gx0 + 2 * SS, mid_y), (gx1 - 2 * SS, mid_y)], fill=(96, 118, 82, 140), width=SS)

        pts = []
        gw = gx1 - gx0 - 6 * SS
        gh = (gy1 - gy0) * 0.40
        for i in range(160):
            t = i / 159.0
            v = eval_cz_wave(wi, (t * 2.0) % 1.0, dcw=0.84)
            pts.append((gx0 + 3 * SS + t * gw, mid_y - v * gh))
        dr.line(pts, fill=LCD_INK, width=int(2.0 * SS), joint="curve")

        fr = im.resize((w, h), Image.LANCZOS)
        strip.paste(fr, (0, wi * h))

    strip.save(os.path.join(SKIN_DIR, filename))


def make_meter_lcd_strip(filename, w=20, h=62):
    """Render slim 8-frame Casio LCD bargraph level meter (0..7 segments)."""
    strip = Image.new("RGBA", (w, h * 8), (0, 0, 0, 0))
    for lvl in range(8):
        im = Image.new("RGBA", (w * SS, h * SS), (0, 0, 0, 0))
        dr = ImageDraw.Draw(im)
        seg_h = 5.4 * SS
        gap = 2.6 * SS
        y_bot = (h - 3) * SS
        for s in range(7):
            y1 = y_bot - s * (seg_h + gap)
            y0 = y1 - seg_h
            col = ((156, 36, 28, 255) if s == 6 else LCD_INK) if s < lvl else LCD_GHOST
            dr.rectangle([2 * SS, y0, (w - 2) * SS, y1], fill=col)
        fr = im.resize((w, h), Image.LANCZOS)
        strip.paste(fr, (0, lvl * h))
    strip.save(os.path.join(SKIN_DIR, filename))


def build_page1():
    img = make_cz101_chassis(seed=101)
    draw_cz101_header(img, "PAGE 1: DCO 1 • LIVE DCW TIMBRE • DCW & DCA ENVELOPES")
    dr = ImageDraw.Draw(img)

    # 2x2 Casio CZ-101 Technical Schematic Grid (Row 1 = Flat CZ Dials, Row 2 = Casio Mini-Sliders)
    draw_cz_schematic_box(img, 26,  136, 590, 224, "DCO 1",      "WAVEFORM & MODULATION",     CZ_TEAL,  "BANK 1")
    draw_cz_schematic_box(img, 664, 136, 590, 224, "DCW & OUT",  "LIVE TIMBRE • CHORUS • VOL", CZ_CORAL, "BANK 2")
    draw_cz_schematic_box(img, 26,  374, 590, 234, "DCW EG",     "PHASE DISTORTION ENVELOPE", CZ_CORAL, "BANK 3")
    draw_cz_schematic_box(img, 664, 374, 590, 234, "DCA EG",     "AMPLITUDE ENVELOPE",        CZ_IVORY, "BANK 4")

    # Signal-flow arrow between DCO 1 -> DCW & OUT
    draw_signal_arrow(img, 622, 658, 254, accent=CZ_TEAL_HI)

    w1_short = ["SAW", "SQR", "PLS", "DBL", "S-PL", "R-SW", "R-TR", "R-TP"]
    w2_short = ["OFF", "SAW", "SQR", "PLS", "DBL", "S-PL", "R-SW", "R-TR", "R-TP"]

    y_row1 = 254
    dials_spec = [
        # Bank 1: DCO 1 (4 flat matte-black CZ dials in one wide row)
        (1, "line_mode",  104, y_row1, 66, "dial_cz_teal_4way.png",  4, "LINE MODE", "choice",
         choice_ticks(["L1", "L2", "1+2", "1+2'"]), -75, 75, CZ_TEAL_HI),
        (2, "l1_wave1",   250, y_row1, 66, "dial_cz_teal_8way.png",  8, "WAVE 1",    "choice",
         choice_ticks(w1_short), -135, 135, CZ_TEAL_HI),
        (3, "l1_wave2",   396, y_row1, 66, "dial_cz_teal_9way.png",  9, "WAVE 2",    "choice",
         choice_ticks(w2_short), -140, 140, CZ_TEAL_HI),
        (4, "mod_mode",   542, y_row1, 66, "dial_cz_coral_4way.png", 4, "RING / NOI","choice",
         choice_ticks(["OFF", "RING", "NOI", "R+N"]), -75, 75, CZ_CORAL_H),

        # Bank 2: DCW TIMBRE & MASTER (4 flat matte-black CZ dials)
        (5,  "dcw_timbre", 742, y_row1, 72, "dial_cz_coral_hero.png", 65, "DCW TIMBRE", "pot", None, -135, 135, CZ_CORAL_H),
        (15, "dcw_env",    888, y_row1, 66, "dial_cz_coral_pot.png",  65, "DCW ENV",    "pot", None, -135, 135, CZ_CORAL_H),
        (14, "chorus",    1034, y_row1, 66, "dial_cz_teal_pot.png",   65, "CHORUS",     "pot", None, -135, 135, CZ_TEAL_HI),
        (0,  "volume",    1180, y_row1, 70, "dial_cz_white_hero.png", 65, "VOLUME",     "pot", None, -135, 135, WHITE),
    ]

    # Row 2: Compact Casio CZ-101 Mini-Sliders (DCW EG & DCA EG)
    y_row2 = 496
    fw, fh = 44, 144
    sliders_spec = [
        # Bank 3: DCW ENVELOPE
        (6,  "dcw_att", 110, y_row2, fw, fh, "slider_cz_coral.png", 65, "ATTACK",  "SLOW", "FAST", False, CZ_CORAL_H),
        (7,  "dcw_dec", 252, y_row2, fw, fh, "slider_cz_coral.png", 65, "DECAY",   "LONG", "SNAP", False, CZ_CORAL_H),
        (8,  "dcw_sus", 394, y_row2, fw, fh, "slider_cz_coral.png", 65, "SUSTAIN", "99",   "0",    False, CZ_CORAL_H),
        (13, "dcw_rel", 536, y_row2, fw, fh, "slider_cz_coral.png", 65, "RELEASE", "LONG", "DRY",  False, CZ_CORAL_H),

        # Bank 4: DCA ENVELOPE
        (9,  "dca_att",  748, y_row2, fw, fh, "slider_cz_ivory.png", 65, "ATTACK",  "SLOW", "FAST", False, CZ_IVORY),
        (10, "dca_dec",  890, y_row2, fw, fh, "slider_cz_ivory.png", 65, "DECAY",   "LONG", "SNAP", False, CZ_IVORY),
        (11, "dca_sus", 1032, y_row2, fw, fh, "slider_cz_ivory.png", 65, "SUSTAIN", "99",   "0",    False, CZ_IVORY),
        (12, "dca_rel", 1174, y_row2, fw, fh, "slider_cz_ivory.png", 65, "RELEASE", "LONG", "DRY",  False, CZ_IVORY),
    ]

    knobs = []
    for pidx, kid, cx, cy, sz, strip, frames, lbl, kind, ticks, a0, a1, acc in dials_spec:
        draw_dial_scale(img, dr, cx, cy, sz * 0.43, lbl, kind=kind, ticks=ticks,
                        start_deg=a0, end_deg=a1, accent=acc, label_y_offset=64 if sz >= 70 else 61)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": sz, "h": sz,
            "strip": strip, "frames": frames, "bipolar": (kind == "bipolar"),
        })

    for pidx, kid, cx, cy, fw_, fh_, strip, frames, lbl, tlbl, blbl, bip, acc in sliders_spec:
        draw_mini_slider_scale(img, dr, cx, cy, fw_, fh_, lbl, top_lbl=tlbl, bot_lbl=blbl, bipolar=bip, accent=acc)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": fw_, "h": fh_,
            "strip": strip, "frames": frames, "bipolar": bip,
        })

    strips = [
        {"param": 2,  "id": "wave_lcd_p1", "cx": 716, "cy": 61, "w": 200, "h": 76, "strip": "wave_lcd_8.png",  "frames": 8},
        {"param": 32, "id": "meter_l_p1",  "cx": 834, "cy": 56, "w": 20,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
        {"param": 33, "id": "meter_r_p1",  "cx": 862, "cy": 56, "w": 20,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
    ]

    labels = [
        {"param": 34, "id": "stat_info_p1", "x": 350, "y": 24, "w": 254, "h": 34, "size": 17, "color": "ff162418"},
        {"param": 35, "id": "stat_wave_p1", "x": 350, "y": 65, "w": 254, "h": 34, "size": 16, "color": "ff162418"},
    ]

    out = img.resize((W, H), Image.LANCZOS).convert("RGB")
    out.save(os.path.join(SKIN_DIR, "bg_czboy.png"))
    return {"knobs": knobs, "strips": strips, "labels": labels}


def build_page2():
    img = make_cz101_chassis(seed=102)
    draw_cz101_header(img, "PAGE 2: LINE 2 DETUNE & LAYER • PITCH EG • VIB & SPACE FX")
    dr = ImageDraw.Draw(img)

    draw_cz_schematic_box(img, 26,  136, 590, 224, "DCO 2",       "LINE 2 WAVE & INTERVAL",     CZ_TEAL,   "BANK 1")
    draw_cz_schematic_box(img, 664, 136, 590, 224, "PITCH / MODE","VOICE • FINE • PITCH EG",    CZ_CORAL,  "BANK 2")
    draw_cz_schematic_box(img, 26,  374, 590, 234, "LINE 2 MIX",  "LAYER BALANCE & ENV OFFSET", CZ_IVORY,  "BANK 3")
    draw_cz_schematic_box(img, 664, 374, 590, 234, "VIB & SPACE", "VIBRATO & AMBIENT SPACE FX", CZ_TEAL,   "BANK 4")

    draw_signal_arrow(img, 622, 658, 254, accent=CZ_CORAL_H)

    w1_short = ["SAW", "SQR", "PLS", "DBL", "S-PL", "R-SW", "R-TR", "R-TP"]
    w2_short = ["OFF", "SAW", "SQR", "PLS", "DBL", "S-PL", "R-SW", "R-TR", "R-TP"]

    y_row1 = 254
    dials_spec = [
        # Bank 1: DCO 2 (4 flat matte-black CZ dials)
        (16, "l2_wave1",  104, y_row1, 66, "dial_cz_teal_8way.png",  8,  "L2 WAVE 1", "choice",
         choice_ticks(w1_short), -135, 135, CZ_TEAL_HI),
        (17, "l2_wave2",  250, y_row1, 66, "dial_cz_teal_9way.png",  9,  "L2 WAVE 2", "choice",
         choice_ticks(w2_short), -140, 140, CZ_TEAL_HI),
        (18, "l2_oct",    396, y_row1, 66, "dial_cz_teal_5way.png",  5,  "L2 OCTAVE", "choice",
         choice_ticks(["-2", "-1", "0", "+1", "+2"]), -100, 100, CZ_TEAL_HI),
        (19, "l2_semi",   542, y_row1, 66, "dial_cz_white_pot.png",  65, "L2 SEMI",   "bipolar", None, -135, 135, CZ_TEAL_HI),

        # Bank 2: VOICE & PITCH EG (4 flat matte-black CZ dials)
        (28, "voice_mode", 742, y_row1, 66, "dial_cz_teal_3way.png",  3,  "VOICE MODE", "choice",
         choice_ticks(["POLY8", "POLY4", "MONO"]), -65, 65, CZ_TEAL_HI),
        (20, "l2_fine",    888, y_row1, 66, "dial_cz_white_pot.png",  65, "L2 FINE",    "bipolar", None, -135, 135, CZ_TEAL_HI),
        (24, "pitch_env", 1034, y_row1, 66, "dial_cz_coral_pot.png",  65, "PITCH ENV",  "pot",     None, -135, 135, CZ_CORAL_H),
        (25, "pitch_dec", 1180, y_row1, 66, "dial_cz_coral_pot.png",  65, "PITCH DEC",  "pot",     None, -135, 135, CZ_CORAL_H),
    ]

    # Row 2: Compact Casio CZ-101 Mini-Sliders (LINE 2 LAYER & VIBRATO / TOUCH)
    y_row2 = 496
    fw, fh = 44, 144
    sliders_spec = [
        # Bank 3: LINE 2 MIX & LAYER
        (21, "l2_mix",     110, y_row2, fw, fh, "slider_cz_teal.png",  65, "L2 MIX",     "99",   "0",    False, CZ_TEAL_HI),
        (22, "l2_dcw",     252, y_row2, fw, fh, "slider_cz_coral.png", 65, "L2 DCW",     "99",   "0",    False, CZ_CORAL_H),
        (23, "l2_env_ofs", 394, y_row2, fw, fh, "slider_cz_ivory.png", 65, "L2 ENV OFS", "SLOW", "SNAP", True,  CZ_IVORY),
        (29, "glide",      536, y_row2, fw, fh, "slider_cz_teal.png",  65, "PORTAMENTO", "99",   "OFF",  False, CZ_TEAL_HI),

        # Bank 4: VIBRATO & AMBIENT SPACE FX
        (26, "vib_rate",     748, y_row2, fw, fh, "slider_cz_ivory.png", 65, "VIB RATE",    "99",   "0",    False, CZ_IVORY),
        (27, "vib_depth",    890, y_row2, fw, fh, "slider_cz_ivory.png", 65, "VIB DEPTH",   "99",   "0",    False, CZ_IVORY),
        (30, "space_send",  1032, y_row2, fw, fh, "slider_cz_coral.png", 65, "SPACE SEND",  "99",   "DRY",  False, CZ_CORAL_H),
        (31, "space_decay", 1174, y_row2, fw, fh, "slider_cz_teal.png",  65, "SPACE DECAY", "WASH", "ROOM", False, CZ_TEAL_HI),
    ]

    knobs = []
    for pidx, kid, cx, cy, sz, strip, frames, lbl, kind, ticks, a0, a1, acc in dials_spec:
        draw_dial_scale(img, dr, cx, cy, sz * 0.43, lbl, kind=kind, ticks=ticks,
                        start_deg=a0, end_deg=a1, accent=acc, label_y_offset=61)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": sz, "h": sz,
            "strip": strip, "frames": frames, "bipolar": (kind == "bipolar"),
        })

    for pidx, kid, cx, cy, fw_, fh_, strip, frames, lbl, tlbl, blbl, bip, acc in sliders_spec:
        draw_mini_slider_scale(img, dr, cx, cy, fw_, fh_, lbl, top_lbl=tlbl, bot_lbl=blbl, bipolar=bip, accent=acc)
        knobs.append({
            "param": pidx, "id": kid, "cx": cx, "cy": cy, "w": fw_, "h": fh_,
            "strip": strip, "frames": frames, "bipolar": bip,
        })

    strips = [
        {"param": 16, "id": "wave_lcd_p2", "cx": 716, "cy": 61, "w": 200, "h": 76, "strip": "wave_lcd_8.png",  "frames": 8},
        {"param": 32, "id": "meter_l_p2",  "cx": 834, "cy": 56, "w": 20,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
        {"param": 33, "id": "meter_r_p2",  "cx": 862, "cy": 56, "w": 20,  "h": 62, "strip": "meter_lcd_8.png", "frames": 8},
    ]

    labels = [
        {"param": 34, "id": "stat_info_p2", "x": 350, "y": 24, "w": 254, "h": 34, "size": 17, "color": "ff162418"},
        {"param": 35, "id": "stat_wave_p2", "x": 350, "y": 65, "w": 254, "h": 34, "size": 16, "color": "ff162418"},
    ]

    out = img.resize((W, H), Image.LANCZOS).convert("RGB")
    out.save(os.path.join(SKIN_DIR, "bg_line2.png"))
    return {"knobs": knobs, "strips": strips, "labels": labels}


def main():
    os.makedirs(SKIN_DIR, exist_ok=True)
    # Clean out any old filmstrips from earlier layouts so skins/ stays tidy
    for fn in os.listdir(SKIN_DIR):
        if fn.endswith(".png"):
            os.remove(os.path.join(SKIN_DIR, fn))

    print("Rendering CZboy Page 1 (1984 Casio CZ-101 Schematic + Silver LCD + Mini-Sliders)...")
    p1 = build_page1()
    print("Rendering CZboy Page 2 (Line 2 Detune / Layer / Pitch EG / Vibrato)...")
    p2 = build_page2()

    print("Rendering 4x-supersampled Flat Matte-Black CZ Dials, Casio Mini-Sliders & LCD Strips...")
    make_wave_lcd_strip("wave_lcd_8.png", 200, 76)
    make_meter_lcd_strip("meter_lcd_8.png", 20, 62)

    # Flat matte-black CZ-style rotary dials
    make_flat_cz_dial("dial_cz_teal_pot.png",    66, 65, (64, 212, 216))
    make_flat_cz_dial("dial_cz_teal_3way.png",   66, 3,  (64, 212, 216), -65, 65)
    make_flat_cz_dial("dial_cz_teal_4way.png",   66, 4,  (64, 212, 216), -75, 75)
    make_flat_cz_dial("dial_cz_teal_5way.png",   66, 5,  (64, 212, 216), -100, 100)
    make_flat_cz_dial("dial_cz_teal_8way.png",   66, 8,  (64, 212, 216), -135, 135)
    make_flat_cz_dial("dial_cz_teal_9way.png",   66, 9,  (64, 212, 216), -140, 140)

    make_flat_cz_dial("dial_cz_coral_hero.png",  72, 65, (246, 104, 78))
    make_flat_cz_dial("dial_cz_coral_pot.png",   66, 65, (246, 104, 78))
    make_flat_cz_dial("dial_cz_coral_4way.png",  66, 4,  (246, 104, 78), -75, 75)

    make_flat_cz_dial("dial_cz_white_hero.png",  70, 65, (240, 240, 236))
    make_flat_cz_dial("dial_cz_white_pot.png",   66, 65, (226, 228, 226))

    # Compact Casio CZ-101 mini-sliders
    make_cz_mini_slider("slider_cz_coral.png", 44, 144, 65, (242, 98, 72))
    make_cz_mini_slider("slider_cz_ivory.png", 44, 144, 65, (228, 220, 192))
    make_cz_mini_slider("slider_cz_teal.png",  44, 144, 65, (64, 208, 212))

    with open(MANIFEST, "w", encoding="utf-8") as fh:
        json.dump({"page1": p1, "page2": p2}, fh, indent=2)
    print(f"Done! Wrote authentic CZ-101 skins to {SKIN_DIR}")


if __name__ == "__main__":
    main()
