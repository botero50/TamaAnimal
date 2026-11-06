/**
 * Tamanimals Localization System
 * 8-language support: English, Spanish, French, German, Japanese, Chinese, Portuguese, Korean
 */

#pragma once
#include <stdint.h>

// ============================================================================
// LANGUAGE ENUMERATION
// ============================================================================

enum : uint8_t {
  LANG_ENGLISH = 0,
  LANG_SPANISH = 1,
  LANG_FRENCH = 2,
  LANG_GERMAN = 3,
  LANG_JAPANESE = 4,
  LANG_CHINESE = 5,
  LANG_PORTUGUESE = 6,
  LANG_KOREAN = 7,
  LANG_COUNT = 8
};

// ============================================================================
// STRING KEYS (For easy translation management)
// ============================================================================

enum : uint16_t {
  // UI Labels
  STR_FEED = 0,
  STR_CLEAN = 1,
  STR_PLAY = 2,
  STR_SLEEP = 3,
  STR_GUIDE = 4,
  STR_SETTINGS = 5,
  STR_STATS = 6,

  // Status Messages
  STR_HUNGRY = 10,
  STR_DIRTY = 11,
  STR_SAD = 12,
  STR_TIRED = 13,
  STR_SICK = 14,
  STR_HAPPY = 15,

  // Game Messages
  STR_WELCOME = 20,
  STR_GAME_OVER = 21,
  STR_LEVEL_UP = 22,
  STR_EVOLVING = 23,

  // Catching
  STR_CAUGHT = 30,
  STR_ESCAPED = 31,
  STR_FAILED = 32,

  // Minigames
  STR_MINIGAME = 40,
  STR_SUCCESS = 41,
  STR_RETRY = 42,
  STR_SCORE = 43,

  // Progression
  STR_TIER = 50,
  STR_PROGRESS = 51,
  STR_ANIMALS = 52,
  STR_NOVICE = 53,
  STR_APPRENTICE = 54,
  STR_AMATEUR = 55,
  STR_INTERMEDIATE = 56,
  STR_ADVANCED = 57,
  STR_EXPERT = 58,
  STR_MASTER = 59,
  STR_LEGEND = 60,
  STR_CHAMPION = 61,

  STR_MAX = 62
};

// ============================================================================
// TRANSLATION TABLES
// ============================================================================

static const char* TRANSLATIONS[LANG_COUNT][STR_MAX] = {
  // ENGLISH
  {
    [STR_FEED] = "FEED",
    [STR_CLEAN] = "CLEAN",
    [STR_PLAY] = "PLAY",
    [STR_SLEEP] = "SLEEP",
    [STR_GUIDE] = "GUIDE",
    [STR_SETTINGS] = "SETTINGS",
    [STR_STATS] = "STATS",

    [STR_HUNGRY] = "Very hungry!",
    [STR_DIRTY] = "Needs cleaning!",
    [STR_SAD] = "Feeling sad",
    [STR_TIRED] = "Very tired",
    [STR_SICK] = "Is sick",
    [STR_HAPPY] = "Very happy!",

    [STR_WELCOME] = "Welcome to Tamanimals!",
    [STR_GAME_OVER] = "Game Over",
    [STR_LEVEL_UP] = "Level Up!",
    [STR_EVOLVING] = "Evolving...",

    [STR_CAUGHT] = "Caught",
    [STR_ESCAPED] = "It escaped!",
    [STR_FAILED] = "Catch failed",

    [STR_MINIGAME] = "Minigame",
    [STR_SUCCESS] = "Success!",
    [STR_RETRY] = "Try again",
    [STR_SCORE] = "Score",

    [STR_TIER] = "Tier",
    [STR_PROGRESS] = "Progress",
    [STR_ANIMALS] = "Animals",
    [STR_NOVICE] = "Novice",
    [STR_APPRENTICE] = "Apprentice",
    [STR_AMATEUR] = "Amateur",
    [STR_INTERMEDIATE] = "Intermediate",
    [STR_ADVANCED] = "Advanced",
    [STR_EXPERT] = "Expert",
    [STR_MASTER] = "Master",
    [STR_LEGEND] = "Legend",
    [STR_CHAMPION] = "Champion",
  },

  // SPANISH
  {
    [STR_FEED] = "ALIMENTAR",
    [STR_CLEAN] = "LIMPIAR",
    [STR_PLAY] = "JUGAR",
    [STR_SLEEP] = "DORMIR",
    [STR_GUIDE] = "GUIDE",
    [STR_SETTINGS] = "AJUSTES",
    [STR_STATS] = "ESTADÍSTICAS",

    [STR_HUNGRY] = "¡Muy hambriento!",
    [STR_DIRTY] = "¡Necesita limpieza!",
    [STR_SAD] = "Se siente triste",
    [STR_TIRED] = "Muy cansado",
    [STR_SICK] = "Está enfermo",
    [STR_HAPPY] = "¡Muy feliz!",

    [STR_WELCOME] = "¡Bienvenido a Tamanimals!",
    [STR_GAME_OVER] = "Fin del juego",
    [STR_LEVEL_UP] = "¡Subida de nivel!",
    [STR_EVOLVING] = "Evolucionando...",

    [STR_CAUGHT] = "Atrapado",
    [STR_ESCAPED] = "¡Se escapó!",
    [STR_FAILED] = "Captura fallida",

    [STR_MINIGAME] = "Minijuego",
    [STR_SUCCESS] = "¡Éxito!",
    [STR_RETRY] = "Intentar de nuevo",
    [STR_SCORE] = "Puntuación",

    [STR_TIER] = "Nivel",
    [STR_PROGRESS] = "Progreso",
    [STR_ANIMALS] = "Animales",
    [STR_NOVICE] = "Novato",
    [STR_APPRENTICE] = "Aprendiz",
    [STR_AMATEUR] = "Amateur",
    [STR_INTERMEDIATE] = "Intermedio",
    [STR_ADVANCED] = "Avanzado",
    [STR_EXPERT] = "Experto",
    [STR_MASTER] = "Maestro",
    [STR_LEGEND] = "Leyenda",
    [STR_CHAMPION] = "Campeón",
  },

  // FRENCH
  {
    [STR_FEED] = "NOURRIR",
    [STR_CLEAN] = "NETTOYER",
    [STR_PLAY] = "JOUER",
    [STR_SLEEP] = "DORMIR",
    [STR_GUIDE] = "GUIDE",
    [STR_SETTINGS] = "PARAMÈTRES",
    [STR_STATS] = "STATISTIQUES",

    [STR_HUNGRY] = "Très affamé!",
    [STR_DIRTY] = "Besoin de nettoyage!",
    [STR_SAD] = "Triste",
    [STR_TIRED] = "Très fatigué",
    [STR_SICK] = "Est malade",
    [STR_HAPPY] = "Très heureux!",

    [STR_WELCOME] = "Bienvenue dans Tamanimals!",
    [STR_GAME_OVER] = "Fin du jeu",
    [STR_LEVEL_UP] = "Montée de niveau!",
    [STR_EVOLVING] = "Évolution...",

    [STR_CAUGHT] = "Attrapé",
    [STR_ESCAPED] = "S'est échappé!",
    [STR_FAILED] = "Capture échouée",

    [STR_MINIGAME] = "Mini-jeu",
    [STR_SUCCESS] = "Succès!",
    [STR_RETRY] = "Réessayer",
    [STR_SCORE] = "Score",

    [STR_TIER] = "Niveau",
    [STR_PROGRESS] = "Progrès",
    [STR_ANIMALS] = "Animaux",
    [STR_NOVICE] = "Novice",
    [STR_APPRENTICE] = "Apprenti",
    [STR_AMATEUR] = "Amateur",
    [STR_INTERMEDIATE] = "Intermédiaire",
    [STR_ADVANCED] = "Avancé",
    [STR_EXPERT] = "Expert",
    [STR_MASTER] = "Maître",
    [STR_LEGEND] = "Légende",
    [STR_CHAMPION] = "Champion",
  },

  // GERMAN
  {
    [STR_FEED] = "FÜTTERN",
    [STR_CLEAN] = "PUTZEN",
    [STR_PLAY] = "SPIELEN",
    [STR_SLEEP] = "SCHLAFEN",
    [STR_GUIDE] = "GUIDE",
    [STR_SETTINGS] = "EINSTELLUNGEN",
    [STR_STATS] = "STATISTIKEN",

    [STR_HUNGRY] = "Sehr hungrig!",
    [STR_DIRTY] = "Braucht Reinigung!",
    [STR_SAD] = "Traurig",
    [STR_TIRED] = "Sehr müde",
    [STR_SICK] = "Ist krank",
    [STR_HAPPY] = "Sehr glücklich!",

    [STR_WELCOME] = "Willkommen bei Tamanimals!",
    [STR_GAME_OVER] = "Spielende",
    [STR_LEVEL_UP] = "Levelaufstieg!",
    [STR_EVOLVING] = "Entwickelt sich...",

    [STR_CAUGHT] = "Gefangen",
    [STR_ESCAPED] = "Es ist entkommen!",
    [STR_FAILED] = "Fang fehlgeschlagen",

    [STR_MINIGAME] = "Minispiel",
    [STR_SUCCESS] = "Erfolg!",
    [STR_RETRY] = "Erneut versuchen",
    [STR_SCORE] = "Punktzahl",

    [STR_TIER] = "Stufe",
    [STR_PROGRESS] = "Fortschritt",
    [STR_ANIMALS] = "Tiere",
    [STR_NOVICE] = "Anfänger",
    [STR_APPRENTICE] = "Lehrling",
    [STR_AMATEUR] = "Amateur",
    [STR_INTERMEDIATE] = "Fortgeschrittene",
    [STR_ADVANCED] = "Erweitert",
    [STR_EXPERT] = "Experte",
    [STR_MASTER] = "Meister",
    [STR_LEGEND] = "Legende",
    [STR_CHAMPION] = "Champion",
  },

  // JAPANESE
  {
    [STR_FEED] = "えさやり",
    [STR_CLEAN] = "きれいに",
    [STR_PLAY] = "あそぶ",
    [STR_SLEEP] = "ねる",
    [STR_GUIDE] = "ずかん",
    [STR_SETTINGS] = "せってい",
    [STR_STATS] = "ステータス",

    [STR_HUNGRY] = "とてもおなかがすいている！",
    [STR_DIRTY] = "きれいにしてほしい！",
    [STR_SAD] = "かなしい",
    [STR_TIRED] = "とても疲れている",
    [STR_SICK] = "病気です",
    [STR_HAPPY] = "とても幸せ！",

    [STR_WELCOME] = "タマニマルズへようこそ！",
    [STR_GAME_OVER] = "ゲームオーバー",
    [STR_LEVEL_UP] = "レベルアップ！",
    [STR_EVOLVING] = "進化中...",

    [STR_CAUGHT] = "つかまった",
    [STR_ESCAPED] = "にげられた！",
    [STR_FAILED] = "つかまえられなかった",

    [STR_MINIGAME] = "ミニゲーム",
    [STR_SUCCESS] = "成功！",
    [STR_RETRY] = "もう一度試す",
    [STR_SCORE] = "スコア",

    [STR_TIER] = "レベル",
    [STR_PROGRESS] = "進捗",
    [STR_ANIMALS] = "動物",
    [STR_NOVICE] = "初心者",
    [STR_APPRENTICE] = "見習い",
    [STR_AMATEUR] = "アマチュア",
    [STR_INTERMEDIATE] = "中級者",
    [STR_ADVANCED] = "上級者",
    [STR_EXPERT] = "エキスパート",
    [STR_MASTER] = "マスター",
    [STR_LEGEND] = "伝説",
    [STR_CHAMPION] = "チャンピオン",
  },

  // CHINESE (Simplified)
  {
    [STR_FEED] = "喂食",
    [STR_CLEAN] = "清洁",
    [STR_PLAY] = "玩耍",
    [STR_SLEEP] = "睡眠",
    [STR_GUIDE] = "图鉴",
    [STR_SETTINGS] = "设置",
    [STR_STATS] = "统计",

    [STR_HUNGRY] = "非常饥饿！",
    [STR_DIRTY] = "需要清洁！",
    [STR_SAD] = "很难过",
    [STR_TIRED] = "很疲倦",
    [STR_SICK] = "生病了",
    [STR_HAPPY] = "非常高兴！",

    [STR_WELCOME] = "欢迎来到动物精灵！",
    [STR_GAME_OVER] = "游戏结束",
    [STR_LEVEL_UP] = "升级了！",
    [STR_EVOLVING] = "进化中...",

    [STR_CAUGHT] = "已捕捉",
    [STR_ESCAPED] = "逃脱了！",
    [STR_FAILED] = "捕捉失败",

    [STR_MINIGAME] = "小游戏",
    [STR_SUCCESS] = "成功！",
    [STR_RETRY] = "重试",
    [STR_SCORE] = "得分",

    [STR_TIER] = "等级",
    [STR_PROGRESS] = "进度",
    [STR_ANIMALS] = "动物",
    [STR_NOVICE] = "新手",
    [STR_APPRENTICE] = "学徒",
    [STR_AMATEUR] = "业余",
    [STR_INTERMEDIATE] = "中级",
    [STR_ADVANCED] = "高级",
    [STR_EXPERT] = "专家",
    [STR_MASTER] = "大师",
    [STR_LEGEND] = "传奇",
    [STR_CHAMPION] = "冠军",
  },

  // PORTUGUESE
  {
    [STR_FEED] = "ALIMENTAR",
    [STR_CLEAN] = "LIMPAR",
    [STR_PLAY] = "JOGAR",
    [STR_SLEEP] = "DORMIR",
    [STR_GUIDE] = "GUIDE",
    [STR_SETTINGS] = "CONFIGURAÇÕES",
    [STR_STATS] = "ESTATÍSTICAS",

    [STR_HUNGRY] = "Muito faminto!",
    [STR_DIRTY] = "Precisa de limpeza!",
    [STR_SAD] = "Triste",
    [STR_TIRED] = "Muito cansado",
    [STR_SICK] = "Está doente",
    [STR_HAPPY] = "Muito feliz!",

    [STR_WELCOME] = "Bem-vindo ao Tamanimals!",
    [STR_GAME_OVER] = "Fim do jogo",
    [STR_LEVEL_UP] = "Subida de nível!",
    [STR_EVOLVING] = "Evoluindo...",

    [STR_CAUGHT] = "Capturado",
    [STR_ESCAPED] = "Escapou!",
    [STR_FAILED] = "Captura falhou",

    [STR_MINIGAME] = "Minijogo",
    [STR_SUCCESS] = "Sucesso!",
    [STR_RETRY] = "Tentar novamente",
    [STR_SCORE] = "Pontuação",

    [STR_TIER] = "Nível",
    [STR_PROGRESS] = "Progresso",
    [STR_ANIMALS] = "Animais",
    [STR_NOVICE] = "Iniciante",
    [STR_APPRENTICE] = "Aprendiz",
    [STR_AMATEUR] = "Amador",
    [STR_INTERMEDIATE] = "Intermediário",
    [STR_ADVANCED] = "Avançado",
    [STR_EXPERT] = "Especialista",
    [STR_MASTER] = "Mestre",
    [STR_LEGEND] = "Lenda",
    [STR_CHAMPION] = "Campeão",
  },

  // KOREAN
  {
    [STR_FEED] = "먹이기",
    [STR_CLEAN] = "청소",
    [STR_PLAY] = "놀기",
    [STR_SLEEP] = "자기",
    [STR_GUIDE] = "도감",
    [STR_SETTINGS] = "설정",
    [STR_STATS] = "통계",

    [STR_HUNGRY] = "매우 배고파!",
    [STR_DIRTY] = "청소가 필요!",
    [STR_SAD] = "슬프다",
    [STR_TIRED] = "매우 피곤",
    [STR_SICK] = "병에 걸렸다",
    [STR_HAPPY] = "매우 행복!",

    [STR_WELCOME] = "타마니말즈에 오신 것을 환영합니다!",
    [STR_GAME_OVER] = "게임 오버",
    [STR_LEVEL_UP] = "레벨 업!",
    [STR_EVOLVING] = "진화 중...",

    [STR_CAUGHT] = "포획됨",
    [STR_ESCAPED] = "도망쳤다!",
    [STR_FAILED] = "포획 실패",

    [STR_MINIGAME] = "미니게임",
    [STR_SUCCESS] = "성공!",
    [STR_RETRY] = "다시 시도",
    [STR_SCORE] = "점수",

    [STR_TIER] = "레벨",
    [STR_PROGRESS] = "진행률",
    [STR_ANIMALS] = "동물",
    [STR_NOVICE] = "초보자",
    [STR_APPRENTICE] = "수습생",
    [STR_AMATEUR] = "아마추어",
    [STR_INTERMEDIATE] = "중급자",
    [STR_ADVANCED] = "고급자",
    [STR_EXPERT] = "전문가",
    [STR_MASTER] = "마스터",
    [STR_LEGEND] = "전설",
    [STR_CHAMPION] = "챔피언",
  },
};

// ============================================================================
// LOCALIZATION SYSTEM
// ============================================================================

uint8_t current_language = LANG_ENGLISH;  // Default language

/**
 * Get translated string
 */
static inline const char* _(uint16_t string_id) {
  if (string_id >= STR_MAX || current_language >= LANG_COUNT) {
    return "???";
  }
  return TRANSLATIONS[current_language][string_id];
}

/**
 * Set current language
 */
static inline void set_language(uint8_t lang_id) {
  if (lang_id < LANG_COUNT) {
    current_language = lang_id;
  }
}

/**
 * Get language name
 */
static inline const char* get_language_name(uint8_t lang_id) {
  const char* names[LANG_COUNT] = {
    "English",
    "Español",
    "Français",
    "Deutsch",
    "日本語",
    "中文",
    "Português",
    "한국어"
  };
  if (lang_id < LANG_COUNT) return names[lang_id];
  return "Unknown";
}

