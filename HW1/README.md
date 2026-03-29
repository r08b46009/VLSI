# HW1: Placement and Routing with Cadence Innovus

## Overview
This project explores the physical design flow using Cadence Innovus for a synthesized standard-cell design.

The experiment focuses on how different settings affect implementation quality, including:
- core utilization
- congestion-driven placement
- timing-driven placement
- slack
- total chip area
- total wirelength

## Task Summary
The original coursework asked students to complete the P&R flow in Innovus, optimize timing/area, compare congestion-driven and timing-driven placement, explain filler-cell insertion, and report slack, area, wirelength, and DRC results. :contentReference[oaicite:2]{index=2}

## My Experimental Settings
- Clock period: 2.5
- Core utilization: 0.96, 0.99
- Congestion-driven effort: high / medium / low
- Timing-driven placement: on / off

## Key Findings
According to my report:
- Increasing utilization from 0.96 to 0.99 reduced total chip area.
- Congestion-driven and timing-driven placement showed trade-offs in slack and wirelength.
- Filler cells were inserted after routing to help maintain continuity and physical completeness. :contentReference[oaicite:3]{index=3}

## Best Result
One reported best result used:
- congestion-driven: high
- timing-driven: off
- slack: 0.02
- total cell area: 39213.082
- total wirelength: 280461.3675 :contentReference[oaicite:4]{index=4}

## Repository Contents
- `report/` — my report PDF
- `images/` — selected layout snapshots
- `scripts/` — optional TCL scripts or command notes
- `notes/` — optional summary notes

## Screenshots

### Best result layout
![Best result layout]([images/best_result.png](https://github.com/r08b46009/VLSI/tree/main/HW1/figures#:~:text=..-,HW1.png,-Add%20files%20via)


## Disclaimer
This repository contains my own write-up and selected results.
Course-distributed assignment specifications and provided design bundles are not included.
