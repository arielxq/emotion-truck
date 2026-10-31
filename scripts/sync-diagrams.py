#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Sync the standalone diagram SVGs into truck/showcase.html.

Each diagram file in diagrams/ is the single source of truth. The matching
<svg> block in showcase.html is located by its aria-labelledby title id and
replaced verbatim.

Usage:
    python3 scripts/sync-diagrams.py
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SHOWCASE = ROOT / "truck" / "showcase.html"
DIAGRAMS = {
    "state-machine.html": "emotion-state-title",
    "timeline.html": "emotion-timeline-title",
    "pie-chart.html": "emotion-pie-title",
    "sankey.html": "emotion-sankey-title",
    "user-journey.html": "emotion-journey-title",
    "architecture.html": "emotion-arch-title",
}

SVG_RE = re.compile(r"<svg\b[^>]*>.*?</svg>", re.DOTALL)


def extract_svg(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    match = SVG_RE.search(text)
    if not match:
        raise SystemExit(f"No <svg> block found in {path}")
    return match.group(0)


def main() -> int:
    html = SHOWCASE.read_text(encoding="utf-8")
    changed = 0

    for filename, title_id in DIAGRAMS.items():
        svg = extract_svg(ROOT / "diagrams" / filename)
        pattern = re.compile(
            r'<svg\b[^>]*aria-labelledby="' + re.escape(title_id) + r'[^"]*"[^>]*>.*?</svg>',
            re.DOTALL,
        )
        if not pattern.search(html):
            print(f"  ! {filename}: no matching SVG in showcase.html", file=sys.stderr)
            continue
        html, count = pattern.subn(lambda _m: svg, html, count=1)
        changed += count
        print(f"  ok {filename} -> {title_id}")

    SHOWCASE.write_text(html, encoding="utf-8")
    print(f"Synced {changed} diagram(s) into {SHOWCASE.relative_to(ROOT)}")
    return 0 if changed == len(DIAGRAMS) else 1


if __name__ == "__main__":
    raise SystemExit(main())
