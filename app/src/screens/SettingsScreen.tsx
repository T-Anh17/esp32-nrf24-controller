import React, { useCallback, useState } from 'react';
import { View, Text, ScrollView, StyleSheet, TextInput, Alert } from 'react-native';
import { useFocusEffect } from '@react-navigation/native';

import { colors, space, radius, font } from '../theme';
import { Card, SectionLabel, Button, Seg } from '../components/ui';
import {
  getBaseUrl,
  setBaseUrl,
  getStatus,
  sendCommand,
  reboot,
} from '../api/client';

const PA = [
  { key: '0', label: 'MAX' },
  { key: '1', label: 'HIGH' },
  { key: '2', label: 'LOW' },
  { key: '3', label: 'MIN' },
];

export default function SettingsScreen() {
  const [url, setUrl] = useState(getBaseUrl());
  const [pa, setPa] = useState('0');
  const [delay, setDelay] = useState('0');
  const [count, setCount] = useState<number | null>(null);
  const [busy, setBusy] = useState(false);

  useFocusEffect(
    useCallback(() => {
      getStatus()
        .then((s) => setCount(s.nrf24_count))
        .catch(() => setCount(null));
    }, [])
  );

  const saveUrl = () => {
    setBaseUrl(url.trim());
    Alert.alert('Đã lưu', `Địa chỉ ESP: ${getBaseUrl()}`);
  };

  const test = async () => {
    setBusy(true);
    try {
      const s = await getStatus();
      Alert.alert('Kết nối OK', `${s.nrf24_count} module nRF24.`);
      setCount(s.nrf24_count);
    } catch {
      Alert.alert('Không kết nối được', 'Kiểm tra đã vào Wi-Fi "jammer" chưa.');
    } finally {
      setBusy(false);
    }
  };

  const applyPa = async (v: string) => {
    setPa(v);
    try {
      await sendCommand(`set pa ${v}`);
    } catch {
      Alert.alert('Lỗi', 'Không gửi được lệnh.');
    }
  };

  const applyDelay = async () => {
    const n = parseInt(delay, 10);
    if (isNaN(n) || n < 0) {
      Alert.alert('Giá trị không hợp lệ', 'Nhập số ≥ 0.');
      return;
    }
    try {
      await sendCommand(`set delay ${n}`);
      Alert.alert('Đã lưu', `Jam delay: ${n} µs`);
    } catch {
      Alert.alert('Lỗi', 'Không gửi được lệnh.');
    }
  };

  const doReboot = () => {
    Alert.alert('Khởi động lại ESP?', '', [
      { text: 'Huỷ', style: 'cancel' },
      { text: 'Reboot', style: 'destructive', onPress: () => reboot().catch(() => {}) },
    ]);
  };

  return (
    <ScrollView
      style={{ flex: 1, backgroundColor: colors.bg }}
      contentContainerStyle={{ padding: space.lg }}
    >
      <SectionLabel>Kết nối</SectionLabel>
      <Card>
        <Text style={styles.label}>Địa chỉ ESP</Text>
        <TextInput
          style={styles.input}
          value={url}
          onChangeText={setUrl}
          autoCapitalize="none"
          autoCorrect={false}
          placeholder="http://192.168.4.1"
          placeholderTextColor={colors.muted}
        />
        <View style={{ flexDirection: 'row', gap: space.md, marginTop: space.md }}>
          <Button label="Lưu" variant="ghost" onPress={saveUrl} style={{ flex: 1 }} />
          <Button label="Kiểm tra" variant="primary" loading={busy} onPress={test} style={{ flex: 1 }} />
        </View>
      </Card>

      <SectionLabel>Phát sóng</SectionLabel>
      <Card>
        <Text style={styles.label}>Mức công suất (PA)</Text>
        <Seg options={PA} value={pa} onChange={applyPa} />
        <View style={{ height: space.lg }} />
        <Text style={styles.label}>Jam delay (µs)</Text>
        <View style={{ flexDirection: 'row', gap: space.md }}>
          <TextInput
            style={[styles.input, { flex: 1 }]}
            value={delay}
            onChangeText={setDelay}
            keyboardType="number-pad"
          />
          <Button label="Lưu" variant="ghost" onPress={applyDelay} />
        </View>
      </Card>

      <SectionLabel>Thiết bị</SectionLabel>
      <Card>
        <View style={styles.kv}>
          <Text style={styles.k}>Module nRF24</Text>
          <Text style={styles.v}>{count == null ? '—' : `${count} module`}</Text>
        </View>
        <View style={styles.kv}>
          <Text style={styles.k}>Địa chỉ hiện tại</Text>
          <Text style={styles.v}>{getBaseUrl().replace('http://', '')}</Text>
        </View>
      </Card>

      <View style={{ height: space.md }} />
      <Button
        label="Khởi động lại ESP"
        variant="danger"
        onPress={doReboot}
      />

      <Text style={styles.note}>
        Cấu hình nâng cao (chân CE/CSN, danh sách SSID, Wi-Fi AP) đặt trực tiếp trong firmware
        hoặc qua lệnh serial. App dùng Wi-Fi HTTP tới ESP.
      </Text>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  label: { color: colors.muted, fontSize: font.sub, marginBottom: 8 },
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
  kv: { flexDirection: 'row', justifyContent: 'space-between', paddingVertical: 5 },
  k: { color: colors.muted, fontSize: font.sub },
  v: { color: colors.text, fontSize: font.sub, fontWeight: '600' },
  note: { color: colors.muted, fontSize: font.tiny, marginTop: space.lg, lineHeight: 17 },
});
