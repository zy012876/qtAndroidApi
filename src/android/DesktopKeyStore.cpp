// Placeholder for desktop builds — KeyStore functionality
// On desktop, SettingsManager handles storage via QSettings + XOR

// This file is compiled only on desktop builds to satisfy linker requirements
// for symbols referenced by AndroidKeyStore.h conditionals.
// No actual implementation needed since AndroidKeyStore methods are stubbed
// in the header's #else branch for non-Android platforms.
