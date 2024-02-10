/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QCEF_WIDGETS_QCEF_WEB_PAGE_H
#define QCEF_WIDGETS_QCEF_WEB_PAGE_H

#include <functional>
#include <QObject>
#include <QIcon>
#include <QUrl>

#include "qcef_ssl_status.h"
#include "qcef_widgets_export.h"

class QWindow;
class QWebChannel;
struct QCefWebPagePrivate;
class QCefWebSettings;
class QCefBrowserEventDelegate;

/**
 * A QCefWebPage object holds web document, history, actions, and provides
 * access to cef browser internal states.
 */
class QCEF_WIDGETS_EXPORT QCefWebPage : public QObject {
  Q_OBJECT

  /**
   * This property holds the title of the web page currently viewed
   * By default, this property contains an empty string.
   */
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)

  /**
   * This property holds the URL of the web page currently viewed
   * Setting this property clears the view and loads the URL.*
   * By default, this property contains an empty, invalid URL.
   */
  Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

  /**
   * This property holds the icon associated with the page currently viewed
   * By default, this property contains a null icon.
   */
  Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)

  /**
   * This property holds the URL of the icon associated with the page currently
   * viewed. By default, this property contains an empty URL.
   */
  Q_PROPERTY(QUrl iconUrl READ iconUrl NOTIFY iconUrlChanged)

  Q_PROPERTY(QString page_error_content
             READ pageErrorContent
             WRITE setPageErrorContent)

  /**
   * This property holds the zoom factor for the page content.
   */
  Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor)

 public:
  explicit QCefWebPage(QObject* parent = nullptr);
  ~QCefWebPage() override;

  void load(const QUrl& url);
  void setUrl(const QUrl& url);

  /**
   * Load the contents of |html| with the specified dummy |url|.
   * |url| should have a standard scheme (for example, http scheme)
   * or behaviors like link clicks and web security restrictions
   * may not behave as expected.
   * @param html Page content.
   * @param url Link of page to be bounded to.
   */
  void setHtml(const QString& html, const QUrl& url);

  QIcon icon() const;
  QUrl iconUrl() const;
  QString title() const;
  QUrl url() const;
  qreal zoomFactor() const;

  void setPageErrorContent(const QString& page_error_content);
  const QString& pageErrorContent() const;

  /**
   * Returns a pointer to the page's settings object.
   * Update browser settings before loading any url or html content or any
   * other methods which will initialize cef window.
   * @return
   */
  QCefWebSettings* settings() const;

  /**
   * Returns a pointer to the web channel instance used by this page.
   * Might be null pointer if it has not been initialized.
   * @return
   */
  QWebChannel* webChannel() const;

  /**
   * Returns the view widget that is associated with the web page.
   * @return
   */
  QWidget* view() const;

  /**
   * Runs the JavaScript code contained in |script_source|.
   * @param script_source
   */
  void runJavaScript(const QString& script_source);

  /**
   * Runs the JavaScript code contained in |script_source|.
   * |script_url| is reference of |script_source| which can be identified
   * in web development console.
   * @param script_source
   * @param script_url
   */
  void runJavaScript(const QString& script_source, const QString& script_url);

  bool canGoBack() const;
  bool canGoForward() const;
  bool isLoading() const;

  typedef std::function<void(const QString& html)> Callback;

  /**
   * Asynchronous method to retrieve the page's content as HTML,
   * enclosed in HTML and BODY tags. Upon successful completion,
   * resultCallback is called with the page's content.
   * @param callback
   */
  void toHtml(Callback callback) const;

  /**
   * Asynchronous method to retrieve the page's content converted
   * to plain text, completely stripped of all HTML formatting.
   * Upon successful completion, resultCallback is called
   * with the page's content.
   * @param callback
   */
  void toPlainText(Callback callback) const;

  /**
   * Change the zoom level to the specified value. Specify 0.0 to reset the
   * zoom level. If called on the UI thread the change will be applied
   * immediately. Otherwise, the change will be applied asynchronously on the
   * UI thread.
   * @param factor
   */
  void setZoomFactor(qreal factor);
  void resetZoomFactor();
  void zoomIn();
  void zoomOut();

  // Editing.
  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void doDelete();
  void selectAll();

  /**
   * Returns ssl status of current page.
   */
  QCefSSLStatus getSSLStatus() const;

  /**
   * Get current event delegate, default is nullptr.
   * Note that QCefWebPage does not take ownership of event delegate.
   * @return
   */
  QCefBrowserEventDelegate* getEventDelegate() const;
  void setEventDelegate(QCefBrowserEventDelegate* delegate);

  bool eventFilter(QObject* watched, QEvent* event) override;

 signals:
  void renderContextCreated();

  void loadStarted();
  void loadingStateChanged(bool is_loading,
                           bool can_go_back,
                           bool can_go_forward);
  void loadFinished(bool ok);

  void fullscreenRequested(bool fullscreen);

  void titleChanged(const QString& title);
  void urlChanged(const QUrl& url);

  /**
   * Notified when page icon is updated.
   */
  void iconChanged(const QIcon& icon);

  /**
   * Notified when page icon is changed.
   * @param icon_url
   */
  void iconUrlChanged(const QUrl& icon_url);

  /**
   * Emit this signal when web notification received from cef browser.
   * @param summary
   * @param title
   */
  void notificationReceived(const QString& summary, const QString& title);

  /**
   * Emitted when cef browser window is closed by javascript.
   * This requires QWebSettings::setJavascriptCloseWindow().
   */
  void windowClosed();

 public slots:
  void back();
  void forward();
  void reload();
  void reloadIgnoreCache();
  void stop();
  void setFocus(bool focus);

 private:
  friend class QCefWebView;
  friend class QCefClientHandlerDelegate;

  void updateBrowserGeometry(const QSize& size);
  void remapBrowserWindow(WId parent_window);

  /**
   * Handle messages received from renderer process.
   */
  void connectTransportChannel();
  void disconnectTransportChannel();
  void handleWebMessage(const QJsonObject& message);

  void updateFavicon(const QUrl& url, const QIcon& icon);
  void updateTitle(const QString& title);
  void updateUrl(const QUrl& url);

  QCefWebPagePrivate* p_ = nullptr;
};


#endif  // QCEF_WIDGETS_QCEF_WEB_PAGE_H
