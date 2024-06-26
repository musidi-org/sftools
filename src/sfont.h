//=============================================================================
//  MuseScore sftools
//
//  Copyright (C) 2004-2011 Werner Schweer
//
//  This work is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  See LICENCE for the licence text and disclaimer of warranty.
//=============================================================================

#ifndef __SOUNDFONT_H__
#define __SOUNDFONT_H__

#include <QtCore/QString>
#include <QtCore/QList>

class Xml;
class QFile;

//---------------------------------------------------------
//   sfVersionTag
//---------------------------------------------------------

struct sfVersionTag
{
      int major;
      int minor;
};

enum Modulator
{
};
enum Generator
{
      Gen_StartAddrOfs,
      Gen_EndAddrOfs,
      Gen_StartLoopAddrOfs,
      Gen_EndLoopAddrOfs,
      Gen_StartAddrCoarseOfs,
      Gen_ModLFO2Pitch,
      Gen_VibLFO2Pitch,
      Gen_ModEnv2Pitch,
      Gen_FilterFc,
      Gen_FilterQ,
      Gen_ModLFO2FilterFc,
      Gen_ModEnv2FilterFc,
      Gen_EndAddrCoarseOfs,
      Gen_ModLFO2Vol,
      Gen_Unused1,
      Gen_ChorusSend,
      Gen_ReverbSend,
      Gen_Pan,
      Gen_Unused2,
      Gen_Unused3,
      Gen_Unused4,
      Gen_ModLFODelay,
      Gen_ModLFOFreq,
      Gen_VibLFODelay,
      Gen_VibLFOFreq,
      Gen_ModEnvDelay,
      Gen_ModEnvAttack,
      Gen_ModEnvHold,
      Gen_ModEnvDecay,
      Gen_ModEnvSustain,
      Gen_ModEnvRelease,
      Gen_Key2ModEnvHold,
      Gen_Key2ModEnvDecay,
      Gen_VolEnvDelay,
      Gen_VolEnvAttack,
      Gen_VolEnvHold,
      Gen_VolEnvDecay,
      Gen_VolEnvSustain,
      Gen_VolEnvRelease,
      Gen_Key2VolEnvHold,
      Gen_Key2VolEnvDecay,
      Gen_Instrument,
      Gen_Reserved1,
      Gen_KeyRange,
      Gen_VelRange,
      Gen_StartLoopAddrCoarseOfs,
      Gen_Keynum,
      Gen_Velocity,
      Gen_Attenuation,
      Gen_Reserved2,
      Gen_EndLoopAddrCoarseOfs,
      Gen_CoarseTune,
      Gen_FineTune,
      Gen_SampleId,
      Gen_SampleModes,
      Gen_Reserved3,
      Gen_ScaleTune,
      Gen_ExclusiveClass,
      Gen_OverrideRootKey,
      Gen_Dummy
};

enum Transform
{
      Linear
};

//---------------------------------------------------------
//   ModulatorList
//---------------------------------------------------------

struct ModulatorList
{
      Modulator src;
      Generator dst;
      int amount;
      Modulator amtSrc;
      Transform transform;
};

//---------------------------------------------------------
//   GeneratorList
//---------------------------------------------------------

union GeneratorAmount
{
      short sword;
      ushort uword;
      struct
      {
            uchar lo, hi;
      };
};

struct GeneratorList
{
      Generator gen;
      GeneratorAmount amount;
};

//---------------------------------------------------------
//   Zone
//---------------------------------------------------------

struct Zone
{
      QList<GeneratorList *> generators;
      QList<ModulatorList *> modulators;
      int instrumentIndex;
};

//---------------------------------------------------------
//   Preset
//---------------------------------------------------------

struct Preset
{
      char *name{0};
      int preset{0};
      int bank{0};
      int presetBagNdx{0}; // used only for read
      int library{0};
      int genre{0};
      int morphology{0};
      QList<Zone *> zones;
};

//---------------------------------------------------------
//   Instrument
//---------------------------------------------------------

struct Instrument
{
      char *name;
      int index; // used only for read
      QList<Zone *> zones;

      Instrument();
      ~Instrument();
};

//---------------------------------------------------------
//   Sample
//---------------------------------------------------------

struct Sample
{
      char *name;
      uint start;
      uint end;
      uint loopstart;
      uint loopend;
      uint samplerate;

      int origpitch;
      int pitchadj;
      int sampletype;

      Sample();
      ~Sample();
};

//---------------------------------------------------------
//   SoundFont
//---------------------------------------------------------

class SoundFont
{
      QString path;
      sfVersionTag version;
      char *engine;
      char *name;
      char *date;
      char *comment;
      char *tools;
      char *creator;
      char *product;
      char *copyright;

      int samplePos;
      int sampleLen;

      QList<Preset *> presets;
      QList<Instrument *> instruments;

      QList<Zone *> pZones;
      QList<Zone *> iZones;
      QList<Sample *> samples;

      QFile *file;
      FILE *f;

      double _oggQuality;
      double _oggAmp;
      qint64 _oggSerial;

      unsigned readDword();
      int readWord();
      int readShort();
      int readByte();
      int readChar();
      int readFourcc(const char *);
      int readFourcc(char *);
      void readSignature(const char *signature);
      void readSignature(char *signature);
      void skip(int);
      void readSection(const char *fourcc, int len);
      void readVersion();
      char *readString(int);
      void readPhdr(int);
      void readBag(int, QList<Zone *> *);
      void readMod(int, QList<Zone *> *);
      void readGen(int, QList<Zone *> *);
      void readInst(int);
      void readShdr(int);

      void writeDword(int);
      void writeWord(unsigned short int);
      void writeByte(unsigned char);
      void writeChar(char);
      void writeShort(short);
      void write(const char *p, int n);
      void write(Xml &, Zone *);
      bool writeSampleFile(Sample *, QString);
      void writeSample(const Sample *);
      void writeStringSection(const char *fourcc, char *s);
      void writePreset(int zoneIdx, const Preset *);
      void writeModulator(const ModulatorList *);
      void writeGenerator(const GeneratorList *);
      void writeInstrument(int zoneIdx, const Instrument *);

      void writeIfil();
      void writeSmpl();
      void writePhdr();
      void writeBag(const char *fourcc, QList<Zone *> *);
      void writeMod(const char *fourcc, const QList<Zone *> *);
      void writeGen(const char *fourcc, QList<Zone *> *);
      void writeInst();
      void writeShdr();

      int writeCompressedSample(Sample *);
      bool writeCSample(Sample *, int);
      char *readCompressedSample(Sample *);

public:
      SoundFont(const QString &);
      ~SoundFont();
      bool read();
      bool write(QFile *, double oggQuality, double oggAmp, qint64 oggSerial);
      bool readXml(QFile *);
      bool writeXml(QFile *);
      bool writeCode(QList<int>);
      bool writeCode();
      void dumpPresets();
};
#endif
