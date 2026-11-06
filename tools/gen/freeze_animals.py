#!/usr/bin/env python3
"""One-shot: turned the first hand-converted species table into animals.json.

Kept as the record of the corrections it applied (typos, duplicates, the 55
UNKNOWN placeholder slots). tools/gen/animals.json is the source of truth now;
gen_dex.py reads that and this script does not need to run again.
"""
import collections
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, '..', '..')

RENAME = {
    'EGYPTION_VULTURE': 'EGYPTIAN VULTURE', 'SUNGREEBE': 'SUNGREBE', 'FINCH_SA': 'SALTATOR FINCH',
    'GREBE_SA': 'SILVERY GREBE', 'PELICAN_SA': 'PERUVIAN PELICAN', 'CORMORANT_SA': 'GUANAY SHAG',
    'PENGUIN_SA': 'MAGELLANIC PENGUIN', 'PUFFIN_SA': 'PIGEON GUILLEMOT', 'PETREL_SA': 'GIANT PETREL',
    'MEGATHERIUM_SA': 'EREMOTHERIUM', 'JAGUAR_EXTINCT': 'GIANT JAGUAR', 'EQUUS_EXTINCT_SA': 'HIPPIDION',
    'CAMELID_EXTINCT': 'HEMIAUCHENIA', 'SABER_TOOTH': 'SMILODON', 'HARPY_EAGLE_MORE': 'SOLITARY EAGLE',
    'ANACONDA_GREEN': 'BENI ANACONDA', 'CRANE_SA': 'TRUMPETER', 'KINGFISHER_SA': 'AMAZON KINGFISHER',
    'CUCKOO_SA': 'SQUIRREL CUCKOO', 'HAWK_SA': 'ROADSIDE HAWK', 'OWL_SA': 'SPECTACLED OWL',
    'WOODPECKER_SA': 'CAMPO FLICKER', 'MACAW_MORE': 'CHESTNUT MACAW', 'MACAW_SCARLET': 'RED FRONT MACAW',
    'MACAW_BLUE': 'SPIX MACAW', 'HYACINTH_MORE': 'LEAR MACAW', 'MACAW_GREEN': 'MILITARY MACAW',
    'TOUCAN_MORE': 'TOCO TOUCAN', 'BIRD_PARADISE': 'CURL CREST JAY', 'CRACIDAE': 'CHACHALACA',
    'RAINFOREST_TURKEY': 'WATTLED GUAN', 'RUFFED_GROUSE': 'WOOD QUAIL', 'BUSHMASTER_SNAKE': 'BUSHMASTER',
    'RAIL_BIRD': 'WOOD RAIL', 'MEGATHEROID': 'NOTHROTHERIUM', 'HIPPO_LEMERLEI': 'DWARF HIPPO',
    'AUSTRALOPITHECUS': 'GELADA', 'HOMO_ERECTUS': 'MANDRILL', 'LESSER_BLACK-BACKED_GULL': 'LESSER GULL',
    'PEARL-SPOTTED_OWLET': 'PEARL OWLET', 'LAPPET-FACED_VULTURE': 'LAPPET VULTURE',
    'WHITE-BACKED_VULTURE': 'WHITEBACK VULTURE', 'LONG-CRESTED_EAGLE': 'CREST EAGLE',
    'LONG-TAILED_PANGOLIN': 'BLACK PANGOLIN', 'AFRICAN_FISH_EAGLE': 'FISH EAGLE',
    'YELLOW-BILLED_STORK': 'YELLOWBILL STORK', 'SADDLE-BILLED_STORK': 'SADDLEBILL STORK',
    'SPINY-TAILED_LIZARD': 'SPINYTAIL LIZARD', 'SOFT-SHELL_TURTLE': 'SOFTSHELL TURTLE',
    'GREEN_TREE_PYTHON': 'GREEN PYTHON', 'ROSEATE_SPOONBILL': 'ROSE SPOONBILL', 'FISH_EAGLE': 'BATELEUR',
    'MEGATHERIUM': 'GIANT SLOTH', 'GIANT_GROUND_SLOTH': 'GROUND SLOTH', 'AFRICAN_WILDDOG': 'CAPE DOG',
    'SPITTING_COBRA': 'SPITTER COBRA', 'CROCODILE_MONITOR': 'SWAMP MONITOR',
    'HOODED_MERGANSER': 'BRAZIL MERGANSER', 'AFRICAN_WILD_CAT': 'WILD CAT', 'LAND_TURTLE': 'LEOPARD TURTLE',
    'PINK_RIVER_DOLPHIN': 'RIVER DOLPHIN', 'POISON_DART_FROG': 'DART FROG', 'SABER-TOOTHED_CAT': 'SABERTOOTH',
    'ANCIENT_LEOPARD': 'CAVE LEOPARD', 'CROWNED_EAGLE': 'CROWNED HAWK',
}

# The 55 slots the first conversion left as UNKNOWN146..200.
ASIA = [
    'TIGER:cat:SPEED:STRENGTH:2', 'GIANT PANDA:bear:SIZE:HERBIVORE:3', 'RED PANDA:small:CLIMB:HERBIVORE:2',
    'SNOW LEOPARD:cat:HIDE:AGILITY:3', 'ASIAN ELEPHANT:elephant:SIZE:INTELLIGENCE:2',
    'ORANGUTAN:primate:INTELLIGENCE:CLIMB:3', 'GIBBON:primate:AGILITY:CLIMB:2', 'MACAQUE:primate:INTELLIGENCE:PACK:1',
    'SUN BEAR:bear:CLIMB:STRENGTH:2', 'SLOTH BEAR:bear:DIG:STRENGTH:2', 'CLOUDED LEOPARD:cat:CLIMB:HIDE:3',
    'DHOLE:dog:PACK:SPEED:2', 'GOLDEN JACKAL:dog:PACK:SPEED:1', 'TANUKI:dog:HIDE:NOCTURNAL:1', 'YAK:bovine:SIZE:HERD:2',
    'BANTENG:bovine:STRENGTH:HERD:2', 'WATER BUFFALO:bovine:STRENGTH:SWIM:1', 'INDIAN RHINO:rhino:ARMOR:SIZE:3',
    'MUSK DEER:antelope:SPEED:HIDE:2', 'SIKA DEER:antelope:SPEED:HERD:1', 'AXIS DEER:antelope:SPEED:HERD:1',
    'SAIGA:antelope:SPEED:HERD:2', 'TAKIN:bovine:SIZE:CLIMB:2', 'WILD BOAR:pig:STRENGTH:DIG:1',
    'MALAYAN TAPIR:pig:SIZE:SWIM:2', 'PEACOCK:bird:SOUND:FLIGHT:1', 'GOLDEN PHEASANT:bird:FLIGHT:HIDE:2',
    'RED CRANE:wader:FLIGHT:SIZE:2', 'MANDARIN DUCK:bird:SWIM:FLIGHT:1', 'BAR GOOSE:bird:FLIGHT:HERD:1',
    'STELLER EAGLE:raptor:FLIGHT:STRENGTH:3', 'EAGLE OWL:raptor:FLIGHT:NOCTURNAL:2', 'LAUGHINGTHRUSH:bird:SOUND:FLIGHT:1',
    'GREAT HORNBILL:parrot:FLIGHT:SOUND:2', 'KOMODO DRAGON:lizard:VENOM:STRENGTH:3', 'KING COBRA:snake:VENOM:STRENGTH:3',
    'RETIC PYTHON:snake:STRENGTH:HIDE:2', 'PIT VIPER:snake:VENOM:HIDE:2', 'TOKAY GECKO:lizard:CLIMB:SOUND:1',
    'FLYING LIZARD:lizard:AGILITY:FLIGHT:2', 'GHARIAL:croc:SWIM:ARMOR:3', 'SALT CROCODILE:croc:STRENGTH:SWIM:3',
    'SPOTTED TURTLE:turtle:ARMOR:SWIM:1', 'GIANT SALAMANDER:frog:SWIM:HIDE:3', 'TREE FROG:frog:CLIMB:SWIM:1',
    'KOI:fish:SWIM:INTELLIGENCE:1', 'SNAKEHEAD:fish:SWIM:SIZE:2', 'BETTA:fish:SWIM:STRENGTH:1',
    'SUN PANGOLIN:armadillo:ARMOR:DIG:2', 'FLYING FOX:bird:FLIGHT:NOCTURNAL:1', 'SLOW LORIS:primate:VENOM:NOCTURNAL:2',
    'TARSIER:primate:AGILITY:NOCTURNAL:2', 'BINTURONG:small:CLIMB:HIDE:2', 'MONGOL HORSE:zebra:SPEED:HERD:2',
    'BACTRIAN CAMEL:camel:SIZE:ARMOR:2',
]

ARCH = [
    (r'LION|CHEETAH|SERVAL|CAT|CARACAL|DINOFELIS|MACHAIRODUS|LEOPARD|JAGUAR|PUMA|OCELOT|MARGAY|SMILODON|SABERTOOTH|THYLACOSMILUS|GENET|CIVET', 'cat'),
    (r'HYENA|JACKAL|WOLF|FOX|DOG|AARDWOLF', 'dog'),
    (r'GIRAFFE', 'giraffe'), (r'ZEBRA|HIPPIDION', 'zebra'), (r'ELEPHANT|DEINOTHERIUM', 'elephant'),
    (r'RHINO', 'rhino'), (r'HIPPO', 'hippo'), (r'BUFFALO|WILDEBEEST|ELAND|TOXODON', 'bovine'),
    (r'IMPALA|GAZELLE|KUDU|ORYX|ANTELOPE|HARTEBEEST|TOPI|BUCK|SITATUNGA|DIK|BROCKET|MACRAUCHENIA', 'antelope'),
    (r'CAMEL|LLAMA|ALPACA|PALEOLAMA|HEMIAUCHENIA', 'camel'), (r'WARTHOG|PIG|PECCARY|TAPIR', 'pig'),
    (r'MONKEY|CHIMP|LEMUR|BABOON|BUSHBABY|GALAGO|GELADA|MANDRILL', 'primate'), (r'BEAR|LEPTARCTUS', 'bear'),
    (r'SLOTH|EREMOTHERIUM|NEOMYLODON|NOTHROTHERIUM|ANKYLOTHERIUM', 'sloth'),
    (r'PANGOLIN|ARMADILLO|GLYPTODON|DOEDICURUS|AARDVARK|ANTEATER', 'armadillo'), (r'DOLPHIN', 'dolphin'),
    (r'MANATEE', 'manatee'), (r'CROCODILE$|CAIMAN', 'croc'), (r'TORTOISE|TURTLE', 'turtle'),
    (r'COBRA|MAMBA|ADDER|PYTHON|VIPER|SNAKE|BOOMSLANG|ANACONDA|BOA|BUSHMASTER|LANCE', 'snake'),
    (r'MONITOR|GECKO|SKINK|AGAMA|IGUANA|CHAMELEON|GOANNA|LIZARD', 'lizard'), (r'FROG', 'frog'),
    (r'PIRANHA', 'fish'), (r'SCORPION', 'scorpion'),
    (r'FLAMINGO|STORK|IBIS|HERON|EGRET|CRANE|SPOONBILL|JABIRU|SHOEBILL|TRUMPETER|HAMERKOP|JACANA|LIMPKIN|SECRETARY', 'wader'),
    (r'OSTRICH|BUSTARD|PHORO|PHORUS', 'flightless'), (r'PENGUIN', 'penguin'),
    (r'EAGLE|VULTURE|CONDOR|HAWK|OWL|BATELEUR', 'raptor'),
    (r'PARROT|MACAW|TOUCAN|HORNBILL|CONURE|PARAKEET|BARBET', 'parrot'),
    (r'BIRD|GULL|KINGFISHER|CORMORANT|DARTER|SKIMMER|GUAN|QUAIL|SCREAMER|TINAMOU|HOATZIN|RAIL|CUCKOO|FLICKER|FLYCATCHER|TANAGER|HONEYEATER|CANARY|FINCH|SPINETAIL|TROGON|MANAKIN|COTINGA|JAY|GREBE|PELICAN|SHAG|BOOBY|ANHINGA|GANNET|GUILLEMOT|PETREL|MERGANSER|CHACHALACA|SUNGREBE', 'bird'),
]
BIOME = {'hippo': 1, 'dolphin': 1, 'manatee': 1, 'croc': 1, 'turtle': 1, 'fish': 1, 'wader': 1, 'pig': 2, 'primate': 2,
         'bear': 2, 'sloth': 2, 'snake': 2, 'frog': 2, 'parrot': 2, 'bird': 2, 'small': 2, 'armadillo': 3, 'lizard': 3,
         'scorpion': 3, 'camel': 4, 'raptor': 4, 'penguin': 5}
TY = {'CLIMB': 'AGILITY', 'HERD': 'PACK', 'NOCTURNAL': 'HIDE', 'DIURNAL': 'SPEED', 'HERBIVORE': 'SIZE',
      'CARNIVORE': 'STRENGTH', 'OMNIVORE': 'AGILITY', 'DEFENSE': 'ARMOR', 'SLOW': 'ARMOR', 'SMALL': 'HIDE', 'NONE': None}


def ty(t):
    t = t.replace('PT_', '')
    return TY.get(t, t)


def arch(n):
    for rx, a in ARCH:
        if re.search(rx, n):
            return a
    return 'small'


def main():
    src = open(os.path.join(ROOT, 'dex.h'), encoding='utf-8').read()
    rows = re.findall(r'\{ "([^"]+)", 0, 0, (\d), 0x[0-9a-fA-F]+, 50, 50, 50, 50, 50, 50, (\d), (PT_\w+), (PT_\w+) \},\s*// (\d+)', src)
    out, seen = [], set()
    for name, rar, _biome, p1, p2, num in rows:
        num = int(num)
        if num == 0:
            continue
        if 146 <= num <= 200:
            n, a, t1, t2, r = ASIA[num - 146].split(':')
            r = int(r)
        else:
            n = RENAME.get(name, name.replace('_', ' ').replace('-', ' '))
            a, t1, t2, r = arch(n), p1, p2, int(rar)
            if a in ('elephant', 'rhino', 'hippo') and r < 2:
                r = 2
            if 131 <= num <= 145 or 236 <= num <= 255:   # prehistoric giants
                r = 4 if r >= 3 else 3
        if n in seen:
            n += ' II'
        seen.add(n)
        t1, t2 = ty(t1), ty(t2)
        if t1 is None:
            t1 = 'STRENGTH'
        if t2 == t1:
            t2 = None
        out.append(dict(id=num, name=n, arch=a, rarity=min(4, max(1, r)), biome=BIOME.get(a, 0), type1=t1, type2=t2))
    assert len(out) == 325, len(out)
    with open(os.path.join(HERE, 'animals.json'), 'w') as f:
        f.write('[\n' + ',\n'.join(json.dumps(o) for o in out) + '\n]\n')
    print('animals', len(out))
    print('long names', [o['name'] for o in out if len(o['name']) > 14])
    print(collections.Counter(o['arch'] for o in out))
    print(collections.Counter(o['rarity'] for o in out))
    print(collections.Counter(o['type1'] for o in out))
    print('small:', [o['name'] for o in out if o['arch'] == 'small'])


main()
