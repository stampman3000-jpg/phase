#!/usr/bin/env python3
"""Generate valid Akai MPC TUI.json and Q-Links.json for CZboy (CZ-101 schematic layout)."""
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKIN_DIR = os.path.join(ROOT, "plugins", "czboy", "skins")
MANIFEST = os.path.join(ROOT, "tools", "ui_manifest.json")


class DupObj:
    def __init__(self, pairs):
        self.pairs = pairs


def dump_mpc_json(obj, indent=0):
    sp = "  " * indent
    sp2 = "  " * (indent + 1)
    if isinstance(obj, DupObj):
        items = [f'{sp2}{json.dumps(k)}: {dump_mpc_json(v, indent + 1)}' for k, v in obj.pairs]
        return "{\n" + ",\n".join(items) + f"\n{sp}}}"
    if isinstance(obj, dict):
        if not obj:
            return "{}"
        items = [f'{sp2}{json.dumps(k)}: {dump_mpc_json(v, indent + 1)}' for k, v in obj.items()]
        return "{\n" + ",\n".join(items) + f"\n{sp}}}"
    if isinstance(obj, list):
        if not obj:
            return "[]"
        if all(not isinstance(x, (dict, list, DupObj)) for x in obj):
            return "[" + ", ".join(json.dumps(x) for x in obj) + "]"
        items = [f"{sp2}{dump_mpc_json(x, indent + 1)}" for x in obj]
        return "[\n" + ",\n".join(items) + f"\n{sp}]"
    return json.dumps(obj)


def empty_bg():
    return {
        "version": 1,
        "focussed":   {"version": 1, "colour": "0", "image": ""},
        "unfocussed": {"version": 1, "colour": "0", "image": ""},
    }


def make_bounds(x, y, w, h, focus="No", invalid="Show", when="Always"):
    return {
        "version": 1,
        "acceptsHWFocus": focus,
        "showWhenDataModelInvalid": invalid,
        "whenVisible": when,
        "boundsType": "Absolute",
        "bounds": f"{int(round(x))} {int(round(y))} {int(round(w))} {int(round(h))}",
    }


def make_entry(name, comp_type, data, bounds, remap=None):
    return {
        "version": 2,
        "componentData": {"version": 1, "name": name, "type": comp_type, "data": data},
        "handle remapping": {"version": 1, "map": remap if remap is not None else []},
        "bounds": bounds,
    }


def make_knob_overlay_def(bipolar=False):
    overlay_data = DupObj([
        ("version", 2),
        ("version", 1),
        ("actions", [
            {"version": 1, "onAction": "Focus Lost", "handler": "Focus Lost", "handleName": "Focus Lost", "additionalData": ""}
        ]),
        ("overlayType", "Knob With Header"),
        ("scaleType", "Linear"),
        ("polarityType", "Bipolar" if bipolar else "Unipolar"),
        ("showMinusSign", True),
        ("handleName", "Data"),
    ])
    return {
        "version": 2,
        "actions": [],
        "backgroundData": empty_bg(),
        "ignoreMousePresses": True,
        "disableCoarseDataWheel": False,
        "componentsData": [
            make_entry("overlay", "KnobOverlay", overlay_data,
                       make_bounds(0, 0, 1280, 800, focus="Yes", invalid="Show"))
        ],
    }


def make_knob_component_def(strip, frames, w, h, bipolar=False):
    overlay_name = "knob overlay bipolar" if bipolar else "knob overlay"
    return {
        "version": 2,
        "actions": [
            {"version": 1, "onAction": "Mouse Down",    "handler": "Q-Link",       "handleName": "Data", "additionalData": ""},
            {"version": 1, "onAction": "Double Click",  "handler": "Show Overlay", "handleName": "Data", "additionalData": overlay_name},
            {"version": 1, "onAction": "Enter Pressed", "handler": "Show Overlay", "handleName": "Data", "additionalData": overlay_name},
        ],
        "backgroundData": empty_bg(),
        "ignoreMousePresses": False,
        "disableCoarseDataWheel": False,
        "componentsData": [
            make_entry(
                "focus", "Focus",
                {"version": 1, "backgroundColour": "141eaeb4", "outlineColour": "ff48d4d8",
                 "backgroundInset": 1.0, "outlineThickness": 1.5},
                make_bounds(0, 0, w, h, focus="No", invalid="Show", when="WhenFocussed"),
            ),
            make_entry(
                "knob", "Knob",
                {"version": 3, "knobType": "FilmStrip", "filmStrip": strip,
                 "numFrames": frames, "invert": False, "dragOrientation": "Vertical",
                 "handleName": "Data"},
                make_bounds(0, 0, w, h, focus="No", invalid="Show"),
            ),
        ],
    }


def make_qlink_dict(banks_left_to_right):
    slots = [
        [13, 9, 5, 1],
        [14, 10, 6, 2],
        [15, 11, 7, 3],
        [16, 12, 8, 4],
    ]
    out = {}
    for b in range(4):
        for k in range(4):
            out[f"Q-Link {slots[b][k]}"] = banks_left_to_right[b][k]
    return {f"Q-Link {i}": out[f"Q-Link {i}"] for i in range(1, 17)}


def main():
    with open(MANIFEST, "r", encoding="utf-8") as fh:
        m = json.load(fh)

    local_defs = [
        {"key": "knob overlay",         "value": make_knob_overlay_def(bipolar=False)},
        {"key": "knob overlay bipolar", "value": make_knob_overlay_def(bipolar=True)},
    ]
    seen = {}

    def knob_type(k):
        key = (k["strip"], k["frames"], k["w"], k["h"], bool(k.get("bipolar", False)))
        if key not in seen:
            tn = f"k_{os.path.splitext(k['strip'])[0]}_{k['w']}x{k['h']}_{'bi' if key[4] else 'uni'}"
            seen[key] = tn
            local_defs.append({
                "key": tn,
                "value": make_knob_component_def(k["strip"], k["frames"], k["w"], k["h"], bipolar=key[4]),
            })
        return seen[key]

    w1_names = ["Saw", "Square", "Pulse", "Dbl Sine", "Saw-Pulse", "Rez Saw", "Rez Tri", "Rez Trap"]
    w2_names = ["Off"] + w1_names
    repl_rows = [
        (1,  ["Line 1", "Line 2", "1 + 2", "1 + 2'"]),
        (2,  w1_names),
        (3,  w2_names),
        (4,  ["Off", "Ring Mod", "Noise", "Ring+N"]),
        (16, w1_names),
        (17, w2_names),
        (18, ["-2", "-1", "0", "+1", "+2"]),
        (28, ["Poly 8", "Poly 4", "Mono"]),
    ]

    tabs = []
    for tname, fn_key, fn_sub, bg_file, spec in [
        ("CZ-101",     0, 0, "bg_czboy.png", m["page1"]),
        ("LINE 2/MOD", 1, 0, "bg_line2.png", m["page2"]),
    ]:
        comps = [
            make_entry("bg", "Image",
                       {"version": 2, "imageType": "Regular", "colour": "0", "image": bg_file},
                       make_bounds(0, 0, 1280, 628, focus="No", invalid="Show"))
        ]
        for st in spec.get("strips", []):
            comps.append(make_entry(
                st["id"], "Image",
                {"version": 2, "imageType": "FilmStrip", "colour": "0",
                 "image": st["strip"], "numFrames": st["frames"],
                 "handleName": f"Parameter {st['param']}"},
                make_bounds(st["cx"] - st["w"]/2, st["cy"] - st["h"]/2, st["w"], st["h"], focus="No", invalid="Show"),
            ))
        for lb in spec.get("labels", []):
            comps.append(make_entry(
                lb["id"], "Label",
                {
                    "version": 1,
                    "textStyle": {
                        "version": 1,
                        "font": {"version": 1, "name": "Titillium Web", "style": "SemiBold", "height": float(lb["size"])},
                        "colour": lb["color"],
                        "justification": "horizontallyCentred verticallyCentred",
                        "case": "Original",
                    },
                    "type": "Value",
                    "handleName": f"Parameter {lb['param']}",
                },
                make_bounds(lb["x"], lb["y"], lb["w"], lb["h"], focus="No", invalid="Show"),
            ))
        for k in spec.get("knobs", []):
            comps.append(make_entry(
                k["id"], knob_type(k),
                {"version": 1, "handleName": "Data"},
                make_bounds(k["cx"] - k["w"]/2, k["cy"] - k["h"]/2, k["w"], k["h"], focus="Yes", invalid="Hide"),
                remap=[{"key": "Data", "value": f"Parameter {k['param']}"}],
            ))

        tabs.append({
            "version": 1,
            "tabName": tname,
            "fnKeyIndex": fn_key,
            "fnKeySubIndex": fn_sub,
            "componentDefinition": {
                "version": 2,
                "actions": [],
                "backgroundData": {"version": 1, "focussed": {"version": 1, "colour": "ff1a1d21", "image": ""}, "unfocussed": {"version": 1, "colour": "ff1a1d21", "image": ""}},
                "ignoreMousePresses": False,
                "disableCoarseDataWheel": False,
                "componentsData": comps,
            },
            "qlinkBoundsData": [],
        })

    tui = {
        "pageData": {
            "version": 1,
            "componentDefinitions": {
                "version": 2,
                "importFiles": [],
                "localComponentDefinitions": local_defs,
            },
            "replacementStrings": [{"key": f"Parameter {p}", "value": v} for p, v in repl_rows],
            "info": {"version": 1, "type": "CompleteDescription"},
            "tabs": tabs,
        }
    }

    qlinks = {
        "version": 4,
        "info": {"version": 1, "type": "CompleteDescription"},
        "Screen Mode Q-Links": {
            "version": 4,
            "map": [
                {
                    "Tab": 1, "SubTab": 1, "Bank Direction": "Column",
                    "Q-Links": make_qlink_dict([
                        [1, 2, 3, 4],     # Bank 1: Line Mode, L1 Wave 1, L1 Wave 2, Ring/Noise
                        [5, 15, 14, 0],   # Bank 2: DCW Timbre, DCW Env, Chorus, Volume
                        [6, 7, 8, 13],    # Bank 3: DCW Attack, Decay, Sustain, Release
                        [9, 10, 11, 12],  # Bank 4: DCA Attack, Decay, Sustain, Release
                    ]),
                },
                {
                    "Tab": 2, "SubTab": 1, "Bank Direction": "Column",
                    "Q-Links": make_qlink_dict([
                        [16, 17, 18, 19], # Bank 1: L2 Wave 1, L2 Wave 2, L2 Octave, L2 Semi
                        [28, 20, 24, 25], # Bank 2: Voice Mode, L2 Fine, Pitch Env, Pitch Dec
                        [21, 22, 23, 29], # Bank 3: L2 Mix, L2 DCW, L2 Env Ofs, Portamento
                        [26, 27, 30, 31], # Bank 4: Vib Rate, Vib Depth, Space Send, Space Decay
                    ]),
                },
            ],
        },
        "Program Mode Q-Links": {
            "Q-Link 1": 5,   # DCW Timbre
            "Q-Link 2": 15,  # DCW Env
            "Q-Link 3": 2,   # L1 Wave 1
            "Q-Link 4": 3,   # L1 Wave 2
            "Q-Link 5": 6,   # DCW Attack
            "Q-Link 6": 7,   # DCW Decay
            "Q-Link 7": 8,   # DCW Sustain
            "Q-Link 8": 13,  # DCW Release
            "Q-Link 9": 9,   # DCA Attack
            "Q-Link 10": 10, # DCA Decay
            "Q-Link 11": 11, # DCA Sustain
            "Q-Link 12": 12, # DCA Release
            "Q-Link 13": 14, # Chorus
            "Q-Link 14": 21, # L2 Mix
            "Q-Link 15": 23, # L2 Env Ofs
            "Q-Link 16": 0,  # Volume
        },
    }

    with open(os.path.join(SKIN_DIR, "TUI.json"), "w", encoding="utf-8") as fh:
        fh.write(dump_mpc_json(tui) + "\n")
    with open(os.path.join(SKIN_DIR, "Q-Links.json"), "w", encoding="utf-8") as fh:
        fh.write(dump_mpc_json(qlinks) + "\n")
    print(f"Wrote TUI.json & Q-Links.json to {SKIN_DIR}")


if __name__ == "__main__":
    main()
