# 🦁 Tamanimals

[![Install in browser](https://img.shields.io/badge/install-in%20browser-FF6B00?logo=googlechrome&logoColor=white)](https://botero50.github.io/TamaAnimal/web/)
![Board](https://img.shields.io/badge/board-ESP32--S3%20round%20AMOLED-E7352C?logo=espressif&logoColor=white)
![Firmware](https://img.shields.io/badge/firmware-v1.3.0-8A2BE2)
![Animals](https://img.shields.io/badge/animals-325%20%2B%2060%20legends-58B868)
![Languages](https://img.shields.io/badge/languages-8-FFCB05)
![Code](https://img.shields.io/badge/code-MIT-blue)
[![Stars](https://img.shields.io/github/stars/botero50/TamaAnimal?style=flat&logo=github&color=yellow)](https://github.com/botero50/TamaAnimal/stargazers)

A virtual pet that lives on a small round touchscreen. Hatch and raise real animals from Africa,
Asia and the Americas, and the creatures of myth and legend, watch them grow up, take care of them,
play minigames, visit places, walk with them, and challenge the park rangers of each region.

**[▶ Install it from your browser](https://botero50.github.io/TamaAnimal/web/)** — Chrome or Edge on
a computer, no Arduino or drivers needed.

<table>
<tr>
<td align="center" width="50%"><img src="docs/tour-play.gif" width="300" alt="Playing: the main screen, the field guide, the rangers, battles, the map, the shop and a minigame"></td>
<td align="center" width="50%"><img src="docs/tour-growth.gif" width="300" alt="Growing up: a lion cub, its card, the grow-up question, then the juvenile and the adult"></td>
</tr>
<tr>
<td align="center"><b>Play</b>: care, explore, battle and collect</td>
<td align="center"><b>Grow</b>: from cub to adult</td>
</tr>
</table>

Every picture on this page is captured from the real firmware by
[`tools/gen/make_docs.py`](tools/gen/make_docs.py), and every animal is drawn by code.

## What you need

| | |
|---|---|
| **Board** | [Waveshare ESP32-S3-Touch-AMOLED-1.75](https://www.waveshare.com/esp32-s3-touch-amoled-1.75.htm) |
| **microSD card** | Any size, for the animal sprites |
| **USB-C cable** | One that carries data, not just charge |

## Installing

1. Open the **[installer](https://botero50.github.io/TamaAnimal/web/)**. Updating a board that has a
   pet? Press **Save my pet** first.
2. Press **Install now** and pick the board's port. A brand-new board needs download mode first:
   hold BOOT, tap RESET, let go of BOOT.
3. Put in a microSD card, press **Connect board**, and send one or more regions. Eggs and wild animals
   come from every region you have sent, mixed.

Updating later? Leave **Erase device** unchecked and your animal, field guide and stamps are kept.

Coming from a version before 1.2.0? Send the sprites again once (step 2) to get the mood faces.
Until you do, the game still runs with the old sprites, just without the new faces.

## The animals

<p align="center"><img src="docs/animals.gif" alt="Twelve of the 325 animals walking"></p>

**325 animals** in three regions, plus **60 legends** in a fourth, each with its own stats, traits,
moves and call. They fall into **14 types** (Strength, Speed, Agility, Smarts, Flight, Swim, Dig,
Stealth, Venom, Armor, Size, Sound, Pack and Spirit), with a matchup chart written for this game.

## Legends

<p align="center"><img src="docs/legends.gif" alt="El Silbon, La Candileja, the Hombre Caiman, La Madremonte, El Sombreron, Kukulkan, Bigfoot, the Loch Ness Monster, the Kraken, Pegasus, the Hydra and the Phoenix"></p>

A fourth region of **60 myths and legends**, with Colombia at its heart: El Silbón, La Llorona,
La Madremonte, La Patasola, El Mohán, La Candileja, El Sombrerón, the Hombre Caimán, the Mula Herrada,
the Muisca serpent of Bachué, the Wayuu weaving spider and the Barranquilla Carnival's Marimonda and
Torito. With them come the Maya, Aztec, Andean and Amazonian legends, and the best-known creatures
of world myth, from Bigfoot and the Loch Ness Monster to Pegasus, the Phoenix and the Kraken.

Its ghosts and spirits bring a 14th type, **Spirit**: strong against Smarts and Sound, weak against
a Pack and Armor, and hard to hurt with brute Strength or Speed. The region has its own rangers, from
Tayrona to the Head Ranger, and its own sprite pack in the installer.

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/guide_legends.png" width="240"> | <img src="docs/screens/ranger_legends.png" width="240"> | <img src="docs/screens/battle_legend.png" width="240"> |
| The Legends field guide | The Legends rangers | A wild eagle of San Agustín |

## Growing up

<p align="center"><img src="docs/growing-up.gif" alt="Cub, juvenile and adult of a lion, an elephant, a giraffe, a flamingo, a crocodile and a panda"></p>

Every animal hatches as a **cub**, becomes a **juvenile** at level 16 and an **adult** at level 36, if
it is well looked after: all its needs must be at 40 or more. Care mistakes, and retiring the
previous pet early, make it grow up later. You can always say "Not yet".

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/main_cub.png" width="240"> | <img src="docs/screens/main_juvenile.png" width="240"> | <img src="docs/screens/main_adult.png" width="240"> |
| A cub at level 10 | A juvenile at level 16 | An adult at level 36 |
| <img src="docs/screens/card_growth.png" width="240"> | <img src="docs/screens/grow_button.png" width="240"> | <img src="docs/screens/grow_dialog.png" width="240"> |
| The card says when it grows up | Ready to grow up | Grow up, or not yet |

## Moods

<p align="center"><img src="docs/moods.gif" alt="A lion cub, a panda and a toucan looking sad, sick, tired, hungry, dirty and happy"></p>

Every animal has its own face for each need: tears when it is sad, a woozy look and a sweat drop
when it is sick, heavy eyes when it is tired, drool when it is hungry, mud when it needs a bath and
a happy little bounce when everything is fine. The most urgent need shows first.

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/mood_happy.png" width="240"> | <img src="docs/screens/mood_hungry.png" width="240"> | <img src="docs/screens/mood_dirty.png" width="240"> |
| Happy: sparkles | Hungry | Dirty: flies and a whiff |
| <img src="docs/screens/mood_tired.png" width="240"> | <img src="docs/screens/mood_sad.png" width="240"> | <img src="docs/screens/mood_sick.png" width="240"> |
| Tired | Sad | Sick |

In battle, a status shows on the fighter itself: burned animals glow and give off embers, poisoned
ones turn purple and bubble, frozen ones go icy and stop moving, sleeping ones lie down, and confused
ones sway under circling stars.

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/battle_burn_poison.png" width="240"> | <img src="docs/screens/battle_freeze_confuse.png" width="240"> | <img src="docs/screens/battle_sleep_para.png" width="240"> |
| Burned and poisoned | Frozen and confused | Asleep |

## Collect and battle

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/guide_regions.png" width="240"> | <img src="docs/screens/guide_africa.png" width="240"> | <img src="docs/screens/card_moves.png" width="240"> |
| The field guide, region by region | Animals you have not met yet are silhouettes | Each animal's moves |
| <img src="docs/screens/battle_wild.png" width="240"> | <img src="docs/screens/battle_ranger.png" width="240"> | <img src="docs/screens/ranger_ladder.png" width="240"> |
| Meet animals in the wild | Battle the rangers | 8 reserve wardens, the Grand 4 and a Head Ranger per region |

## Care and play

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/map.png" width="240"> | <img src="docs/screens/shop.png" width="240"> | <img src="docs/screens/card_stats.png" width="240"> |
| Places to visit | The shop | Stats |
| <img src="docs/screens/game_whois.png" width="240"> | <img src="docs/screens/game_highlow.png" width="240"> | <img src="docs/screens/game_berrydrop.png" width="240"> |
| Who is it? | High or low | Berry drop |

Also: feeding, sleep, baths, chores, furniture, hats, missions, a step counter, the weather, a
12 h or 24 h clock, and a "tap to wake" switch so a pocket cannot wake the screen.

## Eight languages

English, Spanish, French, German, Italian, Portuguese, Chinese and Korean.

| | | |
|:---:|:---:|:---:|
| <img src="docs/screens/card_growth.png" width="240"> | <img src="docs/screens/card_growth_zh.png" width="240"> | <img src="docs/screens/card_growth_ko.png" width="240"> |
| English | 中文 | 한국어 |

## Project status

- ✅ Runs on the board, installed from the web installer.
- ✅ The firmware compiles for the ESP32-S3 with the Arduino esp32 core.
- ✅ The headless test suite runs against the real firmware sources: see [tests](#tests).
- 🚧 [`future/`](future/README.md) holds planned features that are not part of the build yet.

## Building it yourself

You need [arduino-cli](https://arduino.github.io/arduino-cli/) with the `esp32:esp32` core and the
**GFX Library for Arduino**, **SensorLib** and **XPowersLib** libraries.

```bash
python tools/build_web.py
```

That compiles the firmware into `web/firmware/`, writes the manifest and checks that the installer
cannot erase a save. To serve the installer locally, run `python -m http.server` in `web/` and open
`http://localhost:8000`.

## Regenerating the content

Every animal, sprite, stamp, avatar, background, tune and call is generated by code in
[`tools/gen/`](tools/gen), using only the Python standard library.

| Command | Makes |
|---|---|
| `python tools/gen/gen_dex.py` | `dex.h`, `moves.h`, `trainers.h` from `tools/gen/animals.json` and `tools/gen/legends.json` |
| `python tools/gen/gen_art.py` | `badges.h`, `avatars.h`, `backs.h`, `logo.h` and the starter art in `species.h` |
| `python tools/gen/gen_audio.py` | `music.h`, `cries.h` |
| `python tools/gen/gen_sprites.py` | `web/sprites-*.pak` for the microSD: cub, juvenile and adult art for every animal |
| `python tools/gen/gen_cjk.py` | `cjkfont.h`, the Chinese and Korean glyphs the text uses (downloads the font once) |
| `python tools/gen/make_docs.py` | The screens and GIFs on this page, captured from the real firmware (needs `g++`) |
| `python tools/gen/preview.py sheet.png` | A contact sheet of the animals, to check the art by eye |

To add or change an animal, edit `animals.json` (or `legends.json`, where each legend also carries its
own look: colours, wings, extra heads, hats, flames and so on) and rerun these.

## Tests

The suites compile the real firmware against hardware stubs, so they need only `g++`:

```bash
python tools/emu/tests/run_headless.py
python tools/check_ip.py
```

`check_ip.py` fails if any term from another franchise appears in the code, in any language, or inside
the built firmware.

## Credits and license

Code is under the [MIT license](LICENSE). The engine grew out of open-source work by other people,
listed with everything else in [CREDITS.md](CREDITS.md).

Tamanimals is an independent project, not affiliated with or endorsed by any game company.
