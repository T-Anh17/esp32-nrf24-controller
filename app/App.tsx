import React from 'react';
import { StatusBar } from 'expo-status-bar';
import { NavigationContainer, DefaultTheme } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import { Pressable } from 'react-native';
import { Ionicons } from '@expo/vector-icons';

import { colors } from './src/theme';
import HubScreen from './src/screens/HubScreen';
import AttackScreen from './src/screens/AttackScreen';
import WifiScreen from './src/screens/WifiScreen';
import SettingsScreen from './src/screens/SettingsScreen';

export type RootStackParamList = {
  Hub: undefined;
  Attack: { id: string };
  Wifi: undefined;
  Settings: undefined;
};

const Stack = createNativeStackNavigator<RootStackParamList>();

const navTheme = {
  ...DefaultTheme,
  dark: true,
  colors: {
    ...DefaultTheme.colors,
    background: colors.bg,
    card: colors.bg,
    text: colors.text,
    border: colors.border,
    primary: colors.accent,
  },
};

export default function App() {
  return (
    <NavigationContainer theme={navTheme}>
      <StatusBar style="light" />
      <Stack.Navigator
        screenOptions={{
          headerStyle: { backgroundColor: colors.bg },
          headerTintColor: colors.text,
          headerTitleStyle: { fontWeight: '700' },
          headerShadowVisible: false,
          contentStyle: { backgroundColor: colors.bg },
        }}
      >
        <Stack.Screen
          name="Hub"
          component={HubScreen}
          options={({ navigation }) => ({
            title: 'JAMMER',
            headerRight: () => (
              <Pressable
                onPress={() => navigation.navigate('Settings')}
                hitSlop={10}
              >
                <Ionicons name="settings-outline" size={22} color={colors.text} />
              </Pressable>
            ),
          })}
        />
        <Stack.Screen name="Attack" component={AttackScreen} options={{ title: '' }} />
        <Stack.Screen name="Wifi" component={WifiScreen} options={{ title: 'WiFi' }} />
        <Stack.Screen name="Settings" component={SettingsScreen} options={{ title: 'Cài đặt' }} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}
