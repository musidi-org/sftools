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

#include "xml.h"

#include <QtCore/QStringList>

QString docName;

//---------------------------------------------------------
//   Xml
//---------------------------------------------------------

Xml::Xml()
{
      stack.clear();
}

Xml::Xml(QIODevice *device)
    : QTextStream(device)
{
      // setCodec("UTF-8");
      stack.clear();
}

//---------------------------------------------------------
//   putLevel
//---------------------------------------------------------

void Xml::putLevel()
{
      int level = stack.size();
      for (int i = 0; i < level * 2; ++i)
            *this << ' ';
}

//---------------------------------------------------------
//   header
//---------------------------------------------------------

void Xml::header()
{
      *this << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
}

//---------------------------------------------------------
//   stag
//    <mops attribute="value">
//---------------------------------------------------------

void Xml::stag(const QString &s)
{
      putLevel();
      *this << '<' << s << '>' << Qt::endl;
      stack.append(s.split(' ')[0]);
}

//---------------------------------------------------------
//   etag
//    </mops>
//---------------------------------------------------------

void Xml::etag()
{
      putLevel();
      *this << "</" << stack.takeLast() << '>' << Qt::endl;
}

//---------------------------------------------------------
//   tagE
//    <mops attribute="value"/>
//---------------------------------------------------------

void Xml::tagE(const char *format, ...)
{
      va_list args;
      va_start(args, format);
      putLevel();
      *this << '<';
      char buffer[BS];
      vsnprintf(buffer, BS, format, args);
      *this << buffer;
      va_end(args);
      *this << "/>" << Qt::endl;
}

//---------------------------------------------------------
//   tagE
//---------------------------------------------------------

void Xml::tagE(const QString &s)
{
      putLevel();
      *this << '<' << s << "/>\n";
}

//---------------------------------------------------------
//   ntag
//    <mops> without newline
//---------------------------------------------------------

void Xml::ntag(const char *name)
{
      putLevel();
      *this << "<" << name << ">";
}

//---------------------------------------------------------
//   netag
//    </mops>     without indentation
//---------------------------------------------------------

void Xml::netag(const char *s)
{
      *this << "</" << s << '>' << Qt::endl;
}

void Xml::tag(const QString &name, QVariant data)
{
      QString ename(name.split(' ')[0]);

      putLevel();
      switch (data.typeId())
      {
      case QMetaType::Bool:
      case QMetaType::Char:
      case QMetaType::Int:
      case QMetaType::UInt:
            *this << "<" << name << ">";
            *this << data.toInt();
            *this << "</" << ename << ">\n";
            break;
      case QMetaType::Double:
            *this << "<" << name << ">";
            *this << data.value<double>();
            *this << "</" << ename << ">\n";
            break;
      case QMetaType::QString:
            *this << "<" << name << ">";
            *this << xmlString(data.value<QString>());
            *this << "</" << ename << ">\n";
            break;
      default:
            qDebug("Xml::tag: unsupported type %d\n", data.typeId());
            // abort();
            break;
      }
}

//---------------------------------------------------------
//   xmlString
//---------------------------------------------------------

QString Xml::xmlString(const QString &ss)
{
      QString s(ss);
      s.replace('&', "&amp;");
      s.replace('<', "&lt;");
      s.replace('>', "&gt;");
      s.replace('\'', "&apos;");
      s.replace('"', "&quot;");
      return s;
}

//---------------------------------------------------------
//   dump
//---------------------------------------------------------

void Xml::dump(int len, const unsigned char *p)
{
      putLevel();
      int col = 0;
      setFieldWidth(5);
      setNumberFlags(numberFlags() | QTextStream::ShowBase);
      setIntegerBase(16);
      for (int i = 0; i < len; ++i, ++col)
      {
            if (col >= 16)
            {
                  setFieldWidth(0);
                  *this << Qt::endl;
                  col = 0;
                  putLevel();
                  setFieldWidth(5);
            }
            *this << (p[i] & 0xff);
      }
      if (col)
            *this << Qt::endl
                  << Qt::dec;
      setFieldWidth(0);
      setIntegerBase(10);
}

//---------------------------------------------------------
//   printDomElementPath
//---------------------------------------------------------

static QString domElementPath(const QDomElement &e)
{
      QString s;
      QDomNode dn(e);
      while (!dn.parentNode().isNull())
      {
            dn = dn.parentNode();
            const QDomElement &e = dn.toElement();
            const QString k(e.tagName());
            if (!s.isEmpty())
                  s += ":";
            s += k;
      }
      return s;
}

//---------------------------------------------------------
//   domError
//---------------------------------------------------------

void domError(const QDomElement &e)
{
      QString m;
      QString s = domElementPath(e);
      if (!docName.isEmpty())
            m = QString("<%1>:").arg(docName);
      int ln = e.lineNumber();
      if (ln != -1)
            m += QString("line:%1 ").arg(ln);
      int col = e.columnNumber();
      if (col != -1)
            m += QString("col:%1 ").arg(col);
      m += QString("%1: Unknown Node <%2>, type %3").arg(s).arg(e.tagName()).arg(e.nodeType());
      if (e.isText())
            m += QString("  text node <%1>").arg(e.toText().data());
      qDebug("%s", qPrintable(m));
}

//---------------------------------------------------------
//   domNotImplemented
//---------------------------------------------------------

void domNotImplemented(const QDomElement &e)
{
      QString s = domElementPath(e);
      if (!docName.isEmpty())
            qDebug("<%s>:", qPrintable(docName));
      qDebug("%s: Node not implemented: <%s>, type %d\n",
             qPrintable(s), qPrintable(e.tagName()), e.nodeType());
      if (e.isText())
            qDebug("  text node <%s>\n", qPrintable(e.toText().data()));
}

//---------------------------------------------------------
//   writeHtml
//---------------------------------------------------------

void Xml::writeHtml(const QString &s)
{
      QStringList sl(s.split("\n"));
      //
      // remove first line from html (DOCTYPE...)
      //
      for (int i = 1; i < sl.size(); ++i)
            *this << sl[i] << "\n";
}
