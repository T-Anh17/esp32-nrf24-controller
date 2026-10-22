import React, { useCallback, useState } from 'react';
import {
  View,
  Text,
  ScrollView,
  Pressable,
  StyleSheet,
  RefreshControl,
} from 'react-native';
import { useFocusEffect } from '@react-navigation/native';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import { Ionicons } from '@expo/vector-icons';

import { RootStackParamList } from '../../App';
import { colors, space, radius, font } from '../theme';
import { Card, SectionLabel, Pill } from '../components/ui';
import { ATTACKS } from '../api/attacks';
import { getStatus, DeviceStatus, getBaseUrl } from '../api/client';

type Props = NativeStackScreenProps<RootStackParamList, 'Hub'>;

export default function HubScreen({ navigation }: Props) {
  const [status, setStatus] = useState<DeviceStatus | null>(null);
  const [connected, setConnected] = useState(false);
  const [refreshing, setRefreshing] = useState(false);

  const load = useCallback(async () => {
    setRefreshing(true);
    try {
      const s = await getStatus();
      setStatus(s);
      setConnected(true);
    } catch {
      setConnected(false);
    } finally {
      setRefreshing(false);
    }
  }, []);

  useFocusEffect(
    useCallback(() => {
      load();
    }, [load])
  );

  const open = (id: string) => {
    if (id === 'wifi') navigation.navigate('Wifi');
    else navigation.navigate('Attack', { id });
  };

  return (
    <ScrollView
      style={{ flex: 1, backgroundColor: colors.bg }}
      contentContainerStyle={{ padding: space.lg }}
      refreshControl={
        <RefreshControl refreshing={refreshing} onRefresh={load} tintColor={colors.muted} />
      }
    >
      <Card style={styles.statusCard}>
        <View style={styles.statusRow}>
          <Pill
            label={connected ? 'Online' : 'Offline'}
            color={connected ? colors.ok : colors.danger}
            dot={connected ? colors.ok : colors.danger}
          />
          <Text style={styles.meta}>
            {connected && status
              ? `${status.nrf24_count}×nRF24`
              : getBaseUrl().replace('http://', '')}
          </Text>
        </View>
        {!connected && (
          <Text style={styles.hint}>
            Kết nối Wi-Fi <Text style={styles.hintB}>jammer</Text> rồi kéo xuống để làm mới.
          </Text>
        )}
      </Card>

      <SectionLabel>Tấn công</SectionLabel>
      {ATTACKS.map((a) => (
        <Pressable
          key={a.id}
          style={({ pressed }) => [styles.row, pressed && { opacity: 0.8 }]}
          onPress={() => open(a.id)}
        >
          <View style={[styles.rowIcon, { backgroundColor: a.color + '26' }]}>
            <Ionicons name={a.icon as any} size={18} color={a.color} />
          </View>
          <View style={{ flex: 1 }}>
            <Text style={styles.rowName}>{a.name}</Text>
            <Text style={styles.rowSub}>{a.subtitle}</Text>
          </View>
          <Ionicons name="chevron-forward" size={18} color={colors.muted} />
        </Pressable>
      ))}
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  statusCard: { backgroundColor: colors.surface },
  statusRow: { flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between' },
  meta: { color: colors.muted, fontSize: font.tiny },
  hint: { color: colors.muted, fontSize: font.sub, marginTop: space.md, lineHeight: 18 },
  hintB: { color: colors.text, fontWeight: '700' },
  row: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 12,
    backgroundColor: colors.card,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: radius.md,
    padding: space.md,
    marginBottom: 9,
  },
  rowIcon: {
    width: 36,
    height: 36,
    borderRadius: radius.sm,
    alignItems: 'center',
    justifyContent: 'center',
  },
  rowName: { color: colors.text, fontSize: font.body, fontWeight: '600' },
  rowSub: { color: colors.muted, fontSize: font.tiny, marginTop: 2 },
});
