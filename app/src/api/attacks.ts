// Định nghĩa các loại tấn công, ánh xạ sang lệnh text của firmware (/WebCommand).
// Lệnh khớp với grammar serial: jam bt | jam drone | jam ble ad|data |
// jam zigbee | jam wifi all|smart|channel <n> | jam misc <start> <stop> | stop.

export type AttackKind = 'simple' | 'methods' | 'misc' | 'wifi';

export interface Method {
  key: string;
  label: string;
  command: string;
}

export interface Attack {
  id: string;
  name: string;
  subtitle: string;
  icon: string; // Ionicons name
  color: string; // theme color token value
  kind: AttackKind;
  command?: string; // kind 'simple'
  methods?: Method[]; // kind 'methods'
}

import { colors } from '../theme';

export const ATTACKS: Attack[] = [
  {
    id: 'wifi',
    name: 'WiFi',
    subtitle: 'Jam · Deauth · Beacon',
    icon: 'wifi',
    color: colors.accent,
    kind: 'wifi',
  },
  {
    id: 'bluetooth',
    name: 'Bluetooth',
    subtitle: 'Jam BT classic',
    icon: 'bluetooth',
    color: colors.purple,
    kind: 'simple',
    command: 'jam bt',
  },
  {
    id: 'ble',
    name: 'BLE',
    subtitle: 'Advertising · Data',
    icon: 'radio',
    color: colors.ok,
    kind: 'methods',
    methods: [
      { key: 'ad', label: 'Advertising', command: 'jam ble ad' },
      { key: 'data', label: 'Data', command: 'jam ble data' },
    ],
  },
  {
    id: 'drone',
    name: 'Drone',
    subtitle: '2.4GHz control',
    icon: 'airplane',
    color: colors.warn,
    kind: 'simple',
    command: 'jam drone',
  },
  {
    id: 'zigbee',
    name: 'Zigbee',
    subtitle: 'Jam',
    icon: 'git-network',
    color: colors.accent,
    kind: 'simple',
    command: 'jam zigbee',
  },
  {
    id: 'misc',
    name: 'Misc',
    subtitle: 'Dải kênh tuỳ chọn',
    icon: 'options',
    color: colors.muted,
    kind: 'misc',
  },
];

export const getAttack = (id: string): Attack | undefined =>
  ATTACKS.find((a) => a.id === id);
