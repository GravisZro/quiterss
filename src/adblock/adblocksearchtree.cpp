
#include "adblocksearchtree.h"
#include "adblockrule.h"

#include <QDebug>

AdBlockSearchTree::AdBlockSearchTree()
  : m_root(new Node)
{
}

AdBlockSearchTree::~AdBlockSearchTree()
{
  deleteNode(m_root);
}

void AdBlockSearchTree::clear()
{
  deleteNode(m_root);
  m_root = new Node;
}

bool AdBlockSearchTree::add(const AdBlockRule* rule)
{
  if (rule->m_type != AdBlockRule::StringContainsMatchRule) {
    return false;
  }

  const QString filter = rule->m_matchString;
  int len = filter.size();

  if (len <= 0) {
    qDebug() << "AdBlockSearchTree: Inserting rule with filter len <= 0!";
    return false;
  }

  Node* node = m_root;

  for (int i = 0; i < len; ++i) {
    const QChar c = filter.at(i);
    if (!node->children.contains(c)) {
      Node* n = new Node;
      n->c = c;

      node->children[c] = n;
    }

    node = node->children[c];
  }

  node->rule = rule;

  return true;
}

const AdBlockRule* AdBlockSearchTree::find(const QNetworkRequest &request, const QString &domain, const QString &urlString) const
{
  int len = urlString.size();

  if (len <= 0) {
    return 0;
  }

  const QChar* string = urlString.constData();

  for (int i = 0; i < len; ++i) {
    const AdBlockRule* rule = prefixSearch(request, domain, urlString, string++, len - i);
    if (rule) {
      return rule;
    }
  }

  return 0;
}

const AdBlockRule* AdBlockSearchTree::prefixSearch(const QNetworkRequest &request, const QString &domain, const QString &urlString, const QChar* string, int len) const
{
  if (len <= 0) {
    return 0;
  }

  QChar c = string[0];

  if (!m_root->children.contains(c)) {
    return 0;
  }

  Node* node = m_root->children[c];

  for (int i = 1; i < len; ++i) {
    const QChar c = (++string)[0];

    if (node->rule && node->rule->networkMatch(request, domain, urlString)) {
      return node->rule;
    }

    if (!node->children.contains(c)) {
      return 0;
    }

    node = node->children[c];
  }

  if (node->rule && node->rule->networkMatch(request, domain, urlString)) {
    return node->rule;
  }

  return 0;
}

void AdBlockSearchTree::deleteNode(AdBlockSearchTree::Node* node)
{
  if (!node) {
    return;
  }

  QHashIterator<QChar, Node*> i(node->children);
  while (i.hasNext()) {
    i.next();
    deleteNode(i.value());
  }

  delete node;
}
