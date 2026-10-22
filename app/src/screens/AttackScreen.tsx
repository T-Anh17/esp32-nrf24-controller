import React, { useEffect, useRef, useState } from 'react';
import {
  View,
  Text,
  ScrollView,
  StyleSheet,
  TextInput,
  Alert,
} from 'react-native';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import { Ionicons } from '@expo/vector-icons';

import { RootStackParamList } from '../../App';
import { colors, space, radius, font } from '../theme';
import { Card, SectionLabel, Button, Seg, StatusDot } from '../components/ui';
import { getAttack } from '../api/attacks';
import { sendCommand, stopAll } from '../api/client';

type Props = NativeStackScreenProps<RootStackParamList, 'Attack'>;

function fmt(sec: number) {
  const m = Math.floor(sec / 60);
  const s = sec % 60;
  return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
}

export default function AttackScreen({ route, navigation }: Props) {
  const attack = getAttack(route.params.id);
  const [method, setMethod] = useState(attack?.methods?.[0]?.key ?? '');
  const [running, setRunning] = useState(false);
  const [elapsed, setElapsed] = useState(0);
  const [busy, setBusy] = useState(false);
  const [chStart, setChStart] = useState('0');
  const [chStop, setChStop] = useState('80');
  const timer = useRef<ReturnType<typeof setInterval> | null>(null);

  useEffect(() => {
    navigation.setOptions({ title: attack?.name ?? 'Tấn công' });
  }, [navigation, attack]);

  useEffect(() => {
    if (running) {
      timer.current = setInterval(() => setElapsed((e) => e + 1), 1000);
    } else if (timer.current) {
      clearInterval(timer.current);
      timer.current = null;
    }
    return () => {
      if (timer.current) clearInterval(timer.current);
    };
  }, [running]);

  if (!attack) {
    return (
      <View style={styles.center}>
        <Text style={{ color: colors.muted }}>Không tìm thấy loại tấn công.</Text>
      </View>
    );
  }

  const buildCommand = (): string | null => {
    if (attack.kind === 'simple') return attack.command ?? null;
    if (attack.kind === 'methods')
      return attack.methods?.find((m) => m.key === method)?.command ?? null;
    if (attack.kind === 'misc') {
      const a = parseInt(chStart, 10);
      const b = parseInt(chStop, 10);
      if (isNaN(a) || isNaN(b) || a < 0 || b > 80 || a > b) {
        Alert.alert('Kênh không hợp lệ', 'Nhập 0–80, start ≤ stop.');
        return null;
      }
      return `jam misc ${a} ${b}`;
    }
    return null;
  };

  const start = async () => {
    const cmd = buildCommand();
    if (!cmd) return;
    setBusy(true);
    try {
      await sendCommand(cmd);
    } catch {
      // firmware chạy jam loop blocking nên request có thể không trả về — vẫn coi là đã chạy
    } finally {
      setBusy(false);
      setElapsed(0);
      setRunning(true);
    }
  };

  const stop = async () => {
    setBusy(true);
    try {
      await stopAll();
    } catch {
      Alert.alert('Lỗi', 'Không gửi được lệnh dừng. Kiểm tra kết nối Wi-Fi.');
    } finally {
      setBusy(false);
      setRunning(false);
    }
  };

  return (
    <ScrollView
      style={{ flex: 1, backgroundColor: colors.bg }}
      contentContainerStyle={{ padding: space.lg }}
    >
      {running ? (
        <Card style={styles.live}>
          <View style={styles.liveHead}>
            <StatusDot color={colors.danger} />
            <Text style={styles.liveTitle}>ĐANG PHÁT NHIỄU</Text>
            <Text style={styles.elapsed}>{fmt(elapsed)}</Text>
          </View>
          <Text style={styles.liveMeta}>
            {attack.name}
            {attack.kind === 'methods' ? ` · ${method}` : ''}
            {attack.kind === 'misc' ? ` · kênh ${chStart}–${chStop}` : ''}
          </Text>
        </Card>
      ) : (
        <Card style={{ flexDirection: 'row', alignItems: 'center', gap: 12 }}>
          <View style={[styles.icon, { backgroundColor: attack.color + '26' }]}>
            <Ionicons name={attack.icon as any} size={20} color={attack.color} />
          </View>
          <View style={{ flex: 1 }}>
            <Text style={styles.name}>{attack.name}</Text>
            <Text style={styles.sub}>{attack.subtitle}</Text>
          </View>
        </Card>
      )}

      {attack.kind === 'methods' && attack.methods && (
        <>
          <SectionLabel>Phương pháp</SectionLabel>
          <Seg
            options={attack.methods.map((m) => ({ key: m.key, label: m.label }))}
            value={method}
            onChange={setMethod}
          />
        </>
      )}

      {attack.kind === 'misc' && (
        <>
          <SectionLabel>Dải kênh (0–80)</SectionLabel>
          <View style={{ flexDirection: 'row', gap: space.md }}>
            <View style={{ flex: 1 }}>
              <Text style={styles.inLabel}>Từ</Text>
              <TextInput
                style={styles.input}
                value={chStart}
                onChangeText={setChStart}
                keyboardType="number-pad"
                placeholderTextColor={colors.muted}
              />
            </View>
            <View style={{ flex: 1 }}>
              <Text style={styles.inLabel}>Đến</Text>
              <TextInput
                style={styles.input}
                value={chStop}
                onChangeText={setChStop}
                keyboardType="number-pad"
                placeholderTextColor={colors.muted}
              />
            </View>
          </View>
        </>
      )}

      <View style={{ height: space.xl }} />

      {running ? (
        <Button
          label="DỪNG PHÁT"
          variant="stop"
          loading={busy}
          onPress={stop}
          icon={<Ionicons name="stop" size={18} color="#fff" />}
        />
      ) : (
        <Button
          label="BẮT ĐẦU"
          variant="primary"
          loading={busy}
          onPress={start}
          icon={<Ionicons name="flash" size={18} color={colors.accentInk} />}
        />
      )}

      <Text style={styles.note}>
        Lệnh gửi tới ESP: <Text style={styles.mono}>{buildCommand() ?? '—'}</Text>
      </Text>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  center: { flex: 1, alignItems: 'center', justifyContent: 'center', backgroundColor: colors.bg },
  icon: { width: 40, height: 40, borderRadius: radius.sm, alignItems: 'center', justifyContent: 'center' },
  name: { color: colors.text, fontSize: font.h2, fontWeight: '700' },
  sub: { color: colors.muted, fontSize: font.sub, marginTop: 2 },
  live: { borderColor: '#f8514955', backgroundColor: '#f851490d' },
  liveHead: { flexDirection: 'row', alignItems: 'center', gap: 10 },
  liveTitle: { color: colors.danger, fontWeight: '700', fontSize: font.body },
  elapsed: { marginLeft: 'auto', color: colors.muted, fontSize: font.sub },
  liveMeta: { color: colors.muted, fontSize: font.sub, marginTop: 10 },
  inLabel: { color: colors.muted, fontSize: font.tiny, marginBottom: 6 },
  input: {
    backgroundColor: colors.surface,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: radius.sm,
    color: colors.text,
    fontSize: font.body,
    paddingHorizontal: 12,
    paddingVertical: 10,
  },
  note: { color: colors.muted, fontSize: font.tiny, marginTop: space.lg, textAlign: 'center' },
  mono: { color: colors.text, fontFamily: 'monospace' },
});
