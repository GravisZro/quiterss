/**
 * Copyright (c) 2009, Zsombor Gegesy <gzsombor@gmail.com>
 * Copyright (c) 2009, Benjamin C. Meyer <ben@meyerhome.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "adblockrule.h"
#include "adblocksubscription.h"
#include "webpage.h"
#include "common.h"

#include <QDebug>
#include <QUrl>
#include <QString>
#include <QStringList>
#include <QNetworkRequest>
#include <QWebFrame>
#include <QWebPage>

// Version for Qt < 4.8 has one issue, it will wrongly
// count .co.uk (and others) as second-level domain
static QString toSecondLevelDomain(const QUrl &url)
{
#if QT_VERSION >= 0x040800
  const QString urlHost = url.host();
  QStringRef topLevelDomain;
  if(urlHost.lastIndexOf('.') == -1)
    topLevelDomain = urlHost.midRef(urlHost.lastIndexOf('.'));

  if (urlHost.isEmpty() ||
      topLevelDomain.isNull() ||
      topLevelDomain.isEmpty()) {
    return QString();
  }

  QString domain = urlHost.left(urlHost.size() - topLevelDomain.size());

  if (domain.count(QL1C('.')) == 0) {
    return urlHost;
  }

  while (domain.count(QL1C('.')) != 0) {
    domain = domain.mid(domain.indexOf(QL1C('.')) + 1);
  }

  return domain + topLevelDomain;
#else
  QString domain = url.host();

  if (domain.count(QL1C('.')) == 0) {
    return QString();
  }

  while (domain.count(QL1C('.')) != 1) {
    domain = domain.mid(domain.indexOf(QL1C('.')) + 1);
  }

  return domain;
#endif
}

AdBlockRule::AdBlockRule(const QString &filter, AdBlockSubscription* subscription)
  : m_subscription(subscription)
  , m_type(StringContainsMatchRule)
  , m_caseSensitivity(Qt::CaseInsensitive)
  , m_isEnabled(true)
  , m_isException(false)
  , m_isInternalDisabled(false)
  , m_regExp(0)
{
  setFilter(filter);
}

AdBlockRule::~AdBlockRule()
{
  delete m_regExp;
}

AdBlockRule* AdBlockRule::copy() const
{
  AdBlockRule* rule = new AdBlockRule();
  rule->m_subscription = m_subscription;
  rule->m_type = m_type;
  rule->m_options = m_options;
  rule->m_exceptions = m_exceptions;
  rule->m_filter = m_filter;
  rule->m_matchString = m_matchString;
  rule->m_caseSensitivity = m_caseSensitivity;
  rule->m_isEnabled = m_isEnabled;
  rule->m_isException = m_isException;
  rule->m_isInternalDisabled = m_isInternalDisabled;
  rule->m_allowedDomains = m_allowedDomains;
  rule->m_blockedDomains = m_blockedDomains;

  if (m_regExp) {
    rule->m_regExp = new RegExp;
    rule->m_regExp->regExp = m_regExp->regExp;
    rule->m_regExp->matchers = m_regExp->matchers;
  }

  return rule;
}

AdBlockSubscription* AdBlockRule::subscription() const
{
  return m_subscription;
}

void AdBlockRule::setSubscription(AdBlockSubscription* subscription)
{
  m_subscription = subscription;
}

QString AdBlockRule::filter() const
{
  return m_filter;
}

void AdBlockRule::setFilter(const QString &filter)
{
  m_filter = filter;
  parseFilter();
}

bool AdBlockRule::isCssRule() const
{
  return m_type == CssRule;
}

QString AdBlockRule::cssSelector() const
{
  return m_matchString;
}

bool AdBlockRule::isDocument() const
{
  return hasOption(DocumentOption);
}

bool AdBlockRule::isElemhide() const
{
  return hasOption(ElementHideOption);
}

bool AdBlockRule::isDomainRestricted() const
{
  return hasOption(DomainRestrictedOption);
}

bool AdBlockRule::isException() const
{
  return m_isException;
}

bool AdBlockRule::isComment() const
{
  return m_filter.startsWith(QL1C('!'));
}

bool AdBlockRule::isEnabled() const
{
  return m_isEnabled;
}

void AdBlockRule::setEnabled(bool enabled)
{
  m_isEnabled = enabled;
}

bool AdBlockRule::isSlow() const
{
  return m_regExp != 0;
}

bool AdBlockRule::isInternalDisabled() const
{
  return m_isInternalDisabled;
}

bool AdBlockRule::urlMatch(const QUrl &url) const
{
  if (!hasOption(DocumentOption) && !hasOption(ElementHideOption)) {
    return false;
  }

  const QString encodedUrl = url.toEncoded();
  const QString domain = url.host();

  return networkMatch(QNetworkRequest(url), domain, encodedUrl);
}

bool AdBlockRule::networkMatch(const QNetworkRequest &request, const QString &domain, const QString &encodedUrl) const
{
  if (m_type == CssRule || !m_isEnabled || m_isInternalDisabled) {
    return false;
  }

  bool matched = false;

  if (m_type == StringContainsMatchRule) {
    matched = encodedUrl.contains(m_matchString, m_caseSensitivity);
  }
  else if (m_type == DomainMatchRule) {
    matched = isMatchingDomain(domain, m_matchString);
  }
  else if (m_type == StringEndsMatchRule) {
    matched = encodedUrl.endsWith(m_matchString, m_caseSensitivity);
  }
  else if (m_type == RegExpMatchRule) {
    if (!isMatchingRegExpStrings(encodedUrl)) {
      return false;
    }

    matched = (m_regExp->regExp.indexIn(encodedUrl) != -1);
  }

  if (matched) {
    // Check domain restrictions
    if (hasOption(DomainRestrictedOption) && !matchDomain(domain)) {
      return false;
    }

    // Check third-party restriction
    if (hasOption(ThirdPartyOption) && !matchThirdParty(request)) {
      return false;
    }

    // Check object restrictions
    if (hasOption(ObjectOption) && !matchObject(request)) {
      return false;
    }

    // Check subdocument restriction
    if (hasOption(SubdocumentOption) && !matchSubdocument(request)) {
      return false;
    }

    // Check xmlhttprequest restriction
    if (hasOption(XMLHttpRequestOption) && !matchXmlHttpRequest(request)) {
      return false;
    }

    // Check image restriction
    if (hasOption(ImageOption) && !matchImage(encodedUrl)) {
      return false;
    }
  }

  return matched;
}

bool AdBlockRule::matchDomain(const QString &domain) const
{
  if (!m_isEnabled) {
    return false;
  }

  if (!hasOption(DomainRestrictedOption)) {
    return true;
  }

  if (m_blockedDomains.isEmpty()) {
    foreach (const QString &d, m_allowedDomains) {
      if (isMatchingDomain(domain, d)) {
        return true;
      }
    }
  }
  else if (m_allowedDomains.isEmpty()) {
    foreach (const QString &d, m_blockedDomains) {
      if (isMatchingDomain(domain, d)) {
        return false;
      }
    }
    return true;
  }
  else {
    foreach (const QString &d, m_blockedDomains) {
      if (isMatchingDomain(domain, d)) {
        return false;
      }
    }

    foreach (const QString &d, m_allowedDomains) {
      if (isMatchingDomain(domain, d)) {
        return true;
      }
    }
  }

  return false;
}

bool AdBlockRule::matchThirdParty(const QNetworkRequest &request) const
{
  const QString referer = request.attribute(RequestModifiler::RefererString, QString()).toString();

  if (referer.isEmpty()) {
    return false;
  }

  // Third-party matching should be performed on second-level domains
  const QString refererHost = toSecondLevelDomain(QUrl(referer));
  const QString host = toSecondLevelDomain(request.url());

  bool match = refererHost != host;

  return hasException(ThirdPartyOption) ? !match : match;
}

bool AdBlockRule::matchObject(const QNetworkRequest &request) const
{
  bool match = request.attribute(RequestModifiler::TypeString).toString() == QL1S("object");

  return hasException(ObjectOption) ? !match : match;
}

bool AdBlockRule::matchSubdocument(const QNetworkRequest &request) const
{
  QWebFrame* originatingFrame = static_cast<QWebFrame*>(request.originatingObject());
  if (!originatingFrame) {
    return false;
  }

  QWebPage* page = originatingFrame->page();
  if (!page) {
    return false;
  }

  bool match = !(originatingFrame == page->mainFrame());

  return hasException(SubdocumentOption) ? !match : match;
}

bool AdBlockRule::matchXmlHttpRequest(const QNetworkRequest &request) const
{
  bool match = request.rawHeader("X-Requested-With") == QByteArray("XMLHttpRequest");

  return hasException(XMLHttpRequestOption) ? !match : match;
}

bool AdBlockRule::matchImage(const QString &encodedUrl) const
{
  bool match = encodedUrl.endsWith(QL1S(".png")) ||
      encodedUrl.endsWith(QL1S(".jpg")) ||
      encodedUrl.endsWith(QL1S(".gif")) ||
      encodedUrl.endsWith(QL1S(".jpeg"));

  return hasException(ImageOption) ? !match : match;
}

void AdBlockRule::parseFilter()
{
  QString parsedLine = m_filter;

  // Empty rule or just comment
  if (m_filter.trimmed().isEmpty() || m_filter.startsWith(QL1C('!'))) {
    // We want to differentiate rule disabled by user and rule disabled in subscription file
    // m_isInternalDisabled is also used when rule is disabled due to all options not being supported
    m_isEnabled = false;
    m_isInternalDisabled = true;
    m_type = Invalid;
    return;
  }

  // CSS Element hiding rule
  if (parsedLine.contains(QL1S("##")) || parsedLine.contains(QL1S("#@#"))) {
    m_type = CssRule;
    int pos = parsedLine.indexOf(QL1C('#'));

    // Domain restricted rule
    if (!parsedLine.startsWith(QL1S("##"))) {
      QString domains = parsedLine.left(pos);
      parseDomains(domains, QL1C(','));
    }

    m_isException = parsedLine.at(pos + 1) == QL1C('@');
    m_matchString = parsedLine.mid(m_isException ? pos + 3 : pos + 2);

    // CSS rule cannot have more options -> stop parsing
    return;
  }

  // Exception always starts with @@
  if (parsedLine.startsWith(QL1S("@@"))) {
    m_isException = true;
    parsedLine = parsedLine.mid(2);
  }

  // Parse all options following $ char
  int optionsIndex = parsedLine.indexOf(QL1C('$'));
  if (optionsIndex >= 0) {
    const QStringList options = parsedLine.mid(optionsIndex + 1).split(QL1C(','), Qt::SkipEmptyParts);

    int handledOptions = 0;
    foreach (const QString &option, options) {
      if (option.startsWith(QL1S("domain="))) {
        parseDomains(option.mid(7), QL1C('|'));
        ++handledOptions;
      }
      else if (option == QL1S("match-case")) {
        m_caseSensitivity = Qt::CaseSensitive;
        ++handledOptions;
      }
      else if (option.endsWith(QL1S("third-party"))) {
        setOption(ThirdPartyOption);
        setException(ThirdPartyOption, option.startsWith(QL1C('~')));
        ++handledOptions;
      }
      else if (option.endsWith(QL1S("object"))) {
        setOption(ObjectOption);
        setException(ObjectOption, option.startsWith(QL1C('~')));
        ++handledOptions;
      }
      else if (option.endsWith(QL1S("subdocument"))) {
        setOption(SubdocumentOption);
        setException(SubdocumentOption, option.startsWith(QL1C('~')));
        ++handledOptions;
      }
      else if (option.endsWith(QL1S("xmlhttprequest"))) {
        setOption(XMLHttpRequestOption);
        setException(XMLHttpRequestOption, option.startsWith(QL1C('~')));
        ++handledOptions;
      }
      else if (option.endsWith(QL1S("image"))) {
        setOption(ImageOption);
        setException(ImageOption, option.startsWith(QL1C('~')));
        ++handledOptions;
      }
      else if (option == QL1S("document") && m_isException) {
        setOption(DocumentOption);
        ++handledOptions;
      }
      else if (option == QL1S("elemhide") && m_isException) {
        setOption(ElementHideOption);
        ++handledOptions;
      }
      else if (option == QL1S("collapse")) {
        // Hiding placeholders of blocked elements is enabled by default
        ++handledOptions;
      }
    }

    // If we don't handle all options, it's safer to just disable this rule
    if (handledOptions != options.count()) {
      m_isInternalDisabled = true;
      m_type = Invalid;
      return;
    }

    parsedLine = parsedLine.left(optionsIndex);
  }

  // Rule is classic regexp
  if (parsedLine.startsWith(QL1C('/')) && parsedLine.endsWith(QL1C('/'))) {
    parsedLine = parsedLine.mid(1);
    parsedLine = parsedLine.left(parsedLine.size() - 1);

    m_type = RegExpMatchRule;
    m_regExp = new RegExp;
    m_regExp->regExp = QzRegExp(parsedLine, m_caseSensitivity);
    m_regExp->matchers = createStringMatchers(parseRegExpFilter(parsedLine));
    return;
  }

  // Remove starting and ending wildcards (*)
  if (parsedLine.startsWith(QL1C('*'))) {
    parsedLine = parsedLine.mid(1);
  }

  if (parsedLine.endsWith(QL1C('*'))) {
    parsedLine = parsedLine.left(parsedLine.size() - 1);
  }

  // We can use fast string matching for domain here
  if (filterIsOnlyDomain(parsedLine)) {
    parsedLine = parsedLine.mid(2);
    parsedLine = parsedLine.left(parsedLine.size() - 1);

    m_type = DomainMatchRule;
    m_matchString = parsedLine;
    return;
  }

  // If rule contains only | at end, we can also use string matching
  if (filterIsOnlyEndsMatch(parsedLine)) {
    parsedLine = parsedLine.left(parsedLine.size() - 1);

    m_type = StringEndsMatchRule;
    m_matchString = parsedLine;
    return;
  }

  // If we still find a wildcard (*) or separator (^) or (|)
  // we must modify parsedLine to comply with QzRegExp
  if (parsedLine.contains(QL1C('*')) ||
      parsedLine.contains(QL1C('^')) ||
      parsedLine.contains(QL1C('|'))
      ) {
    m_type = RegExpMatchRule;
    m_regExp = new RegExp;
    m_regExp->regExp = QzRegExp(createRegExpFromFilter(parsedLine), m_caseSensitivity);
    m_regExp->matchers = createStringMatchers(parseRegExpFilter(parsedLine));
    return;
  }

  // We haven't found anything that needs use of regexp, yay!
  m_type = StringContainsMatchRule;
  m_matchString = parsedLine;
}

void AdBlockRule::parseDomains(const QString &domains, const QChar &separator)
{
  QStringList domainsList = domains.split(separator, Qt::SkipEmptyParts);

  foreach (const QString domain, domainsList) {
    if (domain.isEmpty()) {
      continue;
    }
    if (domain.startsWith(QL1C('~'))) {
      m_blockedDomains.append(domain.mid(1));
    }
    else {
      m_allowedDomains.append(domain);
    }
  }

  if (!m_blockedDomains.isEmpty() || !m_allowedDomains.isEmpty()) {
    setOption(DomainRestrictedOption);
  }
}

bool AdBlockRule::filterIsOnlyDomain(const QString &filter) const
{
  if (!filter.endsWith(QL1C('^')) || !filter.startsWith(QL1S("||")))
    return false;

  for (int i = 0; i < filter.size(); ++i) {
    switch (filter.at(i).toLatin1()) {
    case '/':
    case ':':
    case '?':
    case '=':
    case '&':
    case '*':
      return false;
    default:
      break;
    }
  }

  return true;
}

bool AdBlockRule::filterIsOnlyEndsMatch(const QString &filter) const
{
  for (int i = 0; i < filter.size(); ++i) {
    switch (filter.at(i).toLatin1()) {
    case '^':
    case '*':
      return false;
    case '|':
      return i == filter.size() - 1;
    default:
      break;
    }
  }

  return false;
}

static bool wordCharacter(const QChar &c)
{
  return c.isLetterOrNumber() || c.isMark() || c == QL1C('_');
}

QString AdBlockRule::createRegExpFromFilter(const QString &filter) const
{
  QString parsed;
  parsed.reserve(filter.size());

  bool hadWildcard = false; // Filter multiple wildcards

  for (int i = 0; i < filter.size(); ++i) {
    const QChar c = filter.at(i);
    switch (c.toLatin1()) {
    case '^':
      parsed.append(QL1S("(?:[^\\w\\d\\-.%]|$)"));
      break;

    case '*':
      if (!hadWildcard)
        parsed.append(QL1S(".*"));
      break;

    case '|':
      if (i == 0) {
        if (filter.size() > 1 && filter.at(1) == QL1C('|')) {
          parsed.append(QL1S("^[\\w\\-]+:\\/+(?!\\/)(?:[^\\/]+\\.)?"));
          i++;
        }
        else {
          parsed.append('^');
        }
        break;
      }
      else if (i == filter.size() - 1) {
        parsed.append(QL1C('$'));
        break;
      }
      // fallthrough

    default:
      if (!wordCharacter(c))
        parsed.append(QL1C('\\') + c);
      else
        parsed.append(c);
    }

    hadWildcard = c == QL1C('*');
  }

  return parsed;
}

QList<QStringMatcher> AdBlockRule::createStringMatchers(const QStringList &filters) const
{
  QList<QStringMatcher> matchers;
  matchers.reserve(filters.size());

  foreach (const QString &filter, filters) {
    matchers.append(QStringMatcher(filter, m_caseSensitivity));
  }

  return matchers;
}

bool AdBlockRule::isMatchingDomain(const QString &domain, const QString &filter) const
{
  return Common::matchDomain(filter, domain);
}

bool AdBlockRule::isMatchingRegExpStrings(const QString &url) const
{
  Q_ASSERT(m_regExp);

  foreach (const QStringMatcher &matcher, m_regExp->matchers) {
    if (matcher.indexIn(url) == -1)
      return false;
  }

  return true;
}

// Split regexp filter into strings that can be used with QString::contains
// Don't use parts that contains only 1 char and duplicated parts
QStringList AdBlockRule::parseRegExpFilter(const QString &filter) const
{
  QStringList list;
  int startPos = -1;

  for (int i = 0; i < filter.size(); ++i) {
    const QChar c = filter.at(i);
    // Meta characters in AdBlock rules are | * ^
    if (c == QL1C('|') || c == QL1C('*') || c == QL1C('^')) {
      const QString sub = filter.mid(startPos, i - startPos);
      if (sub.size() > 1)
        list.append(sub);
      startPos = i + 1;
    }
  }

  const QString sub = filter.mid(startPos);
  if (sub.size() > 1)
    list.append(sub);

  list.removeDuplicates();

  return list;
}

bool AdBlockRule::hasOption(const AdBlockRule::RuleOption &opt) const
{
  return (m_options & opt);
}

bool AdBlockRule::hasException(const AdBlockRule::RuleOption &opt) const
{
  return (m_exceptions & opt);
}

void AdBlockRule::setOption(const AdBlockRule::RuleOption &opt)
{
  m_options |= opt;
}

void AdBlockRule::setException(const AdBlockRule::RuleOption &opt, bool on)
{
  if (on) {
    m_exceptions |= opt;
  }
}
