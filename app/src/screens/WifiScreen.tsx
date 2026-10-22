import React, { useCallback, useState } from 'react';
import {
  View,
  Text,
  ScrollView,
  Pressable,
  StyleSheet,
  ActivityIndicator,
} from 'react-native';
import { useFocusEffect } from '@react-navigation/native';
import { Ionicons } from '@expo/vector-icons';

import { colors, space, radius, font } from '../theme';
import { Card, SectionLabel, Button, Seg, StatusDot } from '../components/ui';
import { wifiAps, sendCommand, stopAll, scanWifi } from '../api/client';

type Tab = 'scan' | 'spam';
interface Ap {
  ssid: string;
  channel: number;
}

export default function WifiScreen() {
  const [tab, setTab] = useState<Tab>('scan');
  const [aps, setAps] = useState<Ap[]>([]);
  const [selected, setSelected] = useState<Set<number>>(new Set());
  const [loading, setLoading] = useState(false);
  const [running, setRunning] = useState<string | null>(null);
  const [busy, setBusy] = useState(false);

  const load = useCallback(async () => {
    setLoading(true);
    try {
      setAps(await wifiAps());
    } catch {
      setAps([]);
    } finally {
      setLoading(false);
    }
  }, []);

  useFocusEffect(
    useCallback(() => {
      load();
    }, [load])
  );

  const rescan = async () => {
    setLoading(true);
    try {
      await scanWifi();
      setAps(await wifiAps());
      setSelected(new Set());
    } catch {
      /* ignore */
    } finally {
      setLoading(false);
    }
  };

  const toggle = (i: number) => {
    setSelected((prev) => {
      const n = new Set(prev);
      n.has(i) ? n.delete(i) : n.add(i);
      return n;
    });
  };

  const selCount = selected.size;
  const firstSelChannel = () => {
    const i = [...selected][0];
    return i != null ? aps[i]?.channel : undefined;
  };

  const fire = async (cmd: string, label: string) => {
    setBusy(true);
    try {
      await sendCommand(cmd);
    } catch {
      /* firmware blocking */
    } finally {
      setBusy(false);
      setRunning(label);
    }
  };

  const stop = async () => {
    setBusy(true);
    try {
      await stopAll();
    } finally {
      setBusy(false);
      setRunning(null);
    }
  };

  const jam = () => {
    const ch = firstSelChannel();
    if (ch) fire(`jam wifi channel ${ch}`, `Jam kênh ${ch}`);
    else fire('jam wifi smart', 'Jam smart (AP đang hoạt động)');
  };
  const deauth = () => {
    const ch = firstSelChannel();
    if (ch) fire(`attack wifi deauth channel ${ch}`, `Deauth kênh ${ch}`);
    else fire('attack wifi deauth smart', 'Deauth smart');
  };

  return (
    <ScrollView
      style={{ flex: 1, backgroundColor: colors.bg }}
      contentContainerStyle={{ padding: space.lg }}
    >
      <Seg
        options={[
          { key: 'scan', label: 'Quét AP' },
          { key: 'spam', label: 'Beacon spam' },
        ]}
        value={tab}
        onChange={(t) => setTab(t as Tab)}
      />

      {running && (
        <Card style={[styles.live, { marginTop: space.md }]}>
          <View style={styles.liveHead}>
            <StatusDot color={colors.danger} />
            <Text style={styles.liveTitle}>{running}</Text>
          </View>
          <Button
            label="DỪNG"
            variant="stop"
            loading={busy}
            onPress={stop}
            style={{ marginTop: space.md }}
            icon={<Ionicons name="stop" size={18} color="#fff" />}
          />
        </Card>
      )}

      {tab === 'scan' && (
        <>
          <View style={styles.scanHead}>
            <SectionLabel>
              {loading ? 'Đang quét…' : `${aps.length} AP · chọn ${selCount}`}
            </SectionLabel>
            <Pressable onPress={rescan} hitSlop={8} style={styles.rescan}>
              <Ionicons name="refresh" size={16} color={colors.accent} />
              <Text style={styles.rescanText}>Quét lại</Text>
            </Pressable>
          </View>

          <Card style={{ padding: 0 }}>
            {loading ? (
              <View style={styles.empty}>
                <ActivityIndicator color={colors.muted} />
              </View>
            ) : aps.length === 0 ? (
              <View style={styles.empty}>
                <Text style={styles.emptyText}>Chưa có AP. Bấm "Quét lại".</Text>
              </View>
            ) : (
              aps.map((ap, i) => {
                const on = selected.has(i);
                return (
                  <Pressable
                    key={i}
                    style={[styles.apRow, i === aps.length - 1 && { borderBottomWidth: 0 }]}
                    onPress={() => toggle(i)}
                  >
                    <View style={[styles.cb, on && styles.cbOn]}>
                      {on && <Ionicons name="checkmark" size={13} color={colors.accentInk} />}
                    </View>
                    <View style={{ flex: 1 }}>
                      <Text style={styles.apName} numberOfLines={1}>
                        {ap.ssid || '(ẩn SSID)'}
                      </Text>
                      <Text style={styles.apSub}>kênh {ap.channel}</Text>
                    </View>
                  </Pressable>
                );
              })
            )}
          </Card>

          <View style={{ flexDirection: 'row', gap: space.md, marginTop: space.lg }}>
            <Button
              label={selCount ? `Jam (${selCount})` : 'Jam smart'}
              variant="primary"
              loading={busy}
              onPress={jam}
              style={{ flex: 1 }}
            />
            <Button
              label={selCount ? `Deauth (${selCount})` : 'Deauth smart'}
              variant="danger"
              loading={busy}
              onPress={deauth}
              style={{ flex: 1 }}
            />
          </View>
        </>
      )}

      {tab === 'spam' && (
        <>
          <SectionLabel>Beacon spam</SectionLabel>
          <Button
            label="Spam SSID ngẫu nhiên"
            variant="primary"
            loading={busy}
            onPress={() => fire('attack wifi spam random', 'Beacon spam (random)')}
            style={{ marginBottom: space.md }}
          />
          <Button
            label="Spam từ danh sách SSID"
            variant="ghost"
            loading={busy}
            onPress={() => fire('attack wifi spam array', 'Beacon spam (list)')}
          />
          <Text style={styles.note}>
            Danh sách SSID chỉnh trong firmware (settings). "Random" tạo tên ngẫu nhiên.
          </Text>
        </>
      )}
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  scanHead: { flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between' },
  rescan: { flexDirection: 'row', alignItems: 'center', gap: 4 },
  rescanText: { color: colors.accent, fontSize: font.sub, fontWeight: '600' },
  empty: { padding: space.xl, alignItems: 'center' },
  emptyText: { color: colors.muted, fontSize: font.sub },
  apRow: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 11,
    paddingHorizontal: 13,
    paddingVertical: 11,
    borderBottomWidth: 1,
    borderBottomColor: colors.border,
  },
  cb: {
    width: 18,
    height: 18,
    borderRadius: 5,
    borderWidth: 1.5,
    borderColor: colors.border,
    alignItems: 'center',
    justifyContent: 'center',
  },
  cbOn: { backgroundColor: colors.accent, borderColor: colors.accent },
  apName: { color: colors.text, fontSize: font.body, fontWeight: '600' },
  apSub: { color: colors.muted, fontSize: font.tiny, marginTop: 2 },
  live: { borderColor: '#f8514955', backgroundColor: '#f851490d' },
  liveHead: { flexDirection: 'row', alignItems: 'center', gap: 10 },
  liveTitle: { color: colors.danger, fontWeight: '700', fontSize: font.body },
  note: { color: colors.muted, fontSize: font.tiny, marginTop: space.lg, lineHeight: 17 },
});
