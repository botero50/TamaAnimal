#!/bin/bash

# TamaAnimal - Emulator Launcher Script
# This script builds and runs the SDL2 emulator

set -e  # Exit on error

echo "╔════════════════════════════════════════════════════════════╗"
echo "║        TamaAnimal - SDL2 Emulator Launcher                 ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EMU_DIR="$SCRIPT_DIR/tools/emu"

echo "📁 Project Directory: $SCRIPT_DIR"
echo "📁 Emulator Directory: $EMU_DIR"
echo ""

# Check if emulator directory exists
if [ ! -d "$EMU_DIR" ]; then
    echo "❌ Error: Emulator directory not found at $EMU_DIR"
    exit 1
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "🔨 STEP 1: Building Emulator..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

cd "$EMU_DIR"

# Check if build.sh exists
if [ ! -f "build.sh" ]; then
    echo "❌ Error: build.sh not found"
    exit 1
fi

# Make build script executable
chmod +x build.sh

# Run build
echo "⏳ Running: ./build.sh"
echo ""
./build.sh

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo ""
echo "✅ Build successful!"
echo ""

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "🎮 STEP 2: Launching Emulator..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Check if executable exists in EMU_DIR
if [ ! -f "$EMU_DIR/tamanimals-emu" ]; then
    echo "❌ Error: tamanimals-emu executable not found at $EMU_DIR/tamanimals-emu"
    exit 1
fi

echo "✅ Emulator executable found at: $EMU_DIR/tamanimals-emu"
echo ""
echo "🚀 Launching emulator..."
echo ""
echo "════════════════════════════════════════════════════════════"
echo "EMULATOR WINDOW OPENING..."
echo ""
echo "What to do in the emulator:"
echo "  1. Click 'New Game' button"
echo "  2. Choose a starter animal (e.g., Lion)"
echo "  3. Click buttons to interact:"
echo "     • Feed - Decrease hunger"
echo "     • Bathe - Increase hygiene"
echo "     • Play - Open minigame (earn coins & bond)"
echo "     • Sleep - Restore energy"
echo "     • Interact - More minigame options"
echo "  4. Check 'Tier' to see progression"
echo ""
echo "Keyboard Controls:"
echo "  • Mouse: Click on UI elements"
echo "  • Arrow Keys: Navigate menus"
echo "  • Enter: Select/Confirm"
echo "  • Esc: Back"
echo "  • Space: Toggle speed (1x/2x)"
echo "  • P: Pause/Resume"
echo "  • S: Screenshot"
echo "  • Q: Quit"
echo "════════════════════════════════════════════════════════════"
echo ""

# Run the emulator
cd "$EMU_DIR"
./tamanimals-emu

echo ""
echo "✅ Emulator closed."
echo "👋 Thank you for playing TamaAnimal!"
