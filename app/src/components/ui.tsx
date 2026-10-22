import React from 'react';
import {
  View,
  Text,
  StyleSheet,
  Pressable,
  ViewStyle,
  StyleProp,
  ActivityIndicator,
} from 'react-native';
import { colors, radius, space, font } from '../theme';

export function Card({
  children,
  style,
}: {
  children: React.ReactNode;
  style?: StyleProp<ViewStyle>;
}) {
  return <View style={[styles.card, style]}>{children}</View>;
}

export function SectionLabel({ children }: { children: React.ReactNode }) {
  return <Text style={styles.section}>{children}</Text>;
}

export function StatusDot({ color }: { color: string }) {
  return <View style={[styles.dot, { backgroundColor: color }]} />;
}

export function Pill({
  label,
  color = colors.muted,
  dot,
}: {
  label: string;
  color?: string;
  dot?: string;
}) {
  return (
    <View style={styles.pill}>
      {dot ? <StatusDot color={dot} /> : null}
      <Text style={[styles.pillText, { color }]}>{label}</Text>
    </View>
  );
}

type BtnVariant = 'primary' | 'stop' | 'ghost' | 'danger';

export function Button({
  label,
  onPress,
  variant = 'primary',
  icon,
  loading,
  style,
}: {
  label: string;
  onPress?: () => void;
  variant?: BtnVariant;
  icon?: React.ReactNode;
  loading?: boolean;
  style?: StyleProp<ViewStyle>;
}) {
  const v = btnStyles[variant];
  return (
    <Pressable
      onPress={onPress}
      disabled={loading}
      style={({ pressed }) => [
        styles.btn,
        v.box,
        pressed && { opacity: 0.85 },
        style,
      ]}
    >
      {loading ? (
        <ActivityIndicator color={v.text.color} />
      ) : (
        <>
          {icon}
          <Text style={[styles.btnText, v.text]}>{label}</Text>
        </>
      )}
    </Pressable>
  );
}

export function Seg<T extends string>({
  options,
  value,
  onChange,
}: {
  options: { key: T; label: string }[];
  value: T;
  onChange: (k: T) => void;
}) {
  return (
    <View style={styles.seg}>
      {options.map((o) => {
        const on = o.key === value;
        return (
          <Pressable
            key={o.key}
            style={[styles.segItem, on && styles.segItemOn]}
            onPress={() => onChange(o.key)}
          >
            <Text style={[styles.segText, on && styles.segTextOn]}>
              {o.label}
            </Text>
          </Pressable>
        );
      })}
    </View>
  );
}

const styles = StyleSheet.create({
  card: {
    backgroundColor: colors.card,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: radius.md,
    padding: space.md,
  },
  section: {
    color: colors.muted,
    fontSize: font.tiny,
    fontWeight: '600',
    textTransform: 'uppercase',
    letterSpacing: 0.5,
    marginTop: space.lg,
    marginBottom: space.sm,
    marginHorizontal: 2,
  },
  dot: { width: 8, height: 8, borderRadius: 4 },
  pill: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 6,
    alignSelf: 'flex-start',
    backgroundColor: colors.surface,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: radius.pill,
    paddingHorizontal: 10,
    paddingVertical: 4,
  },
  pillText: { fontSize: font.tiny },
  btn: {
    height: 48,
    borderRadius: radius.md,
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    gap: 8,
    borderWidth: 1,
    borderColor: 'transparent',
  },
  btnText: { fontSize: font.body, fontWeight: '700' },
  seg: {
    flexDirection: 'row',
    backgroundColor: colors.surface,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: radius.sm,
    overflow: 'hidden',
  },
  segItem: { flex: 1, paddingVertical: 8, alignItems: 'center' },
  segItemOn: { backgroundColor: colors.accent },
  segText: { color: colors.muted, fontSize: font.sub },
  segTextOn: { color: colors.accentInk, fontWeight: '700' },
});

const btnStyles: Record<BtnVariant, { box: ViewStyle; text: { color: string } }> = {
  primary: { box: { backgroundColor: colors.accent }, text: { color: colors.accentInk } },
  stop: { box: { backgroundColor: colors.danger }, text: { color: '#fff' } },
  ghost: {
    box: { backgroundColor: 'transparent', borderColor: colors.border },
    text: { color: colors.text },
  },
  danger: {
    box: { backgroundColor: 'transparent', borderColor: '#f8514955' },
    text: { color: colors.danger },
  },
};
