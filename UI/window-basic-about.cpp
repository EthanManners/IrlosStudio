#include "moc_window-basic-about.cpp"
#include "obs-app.hpp"
#include <qt-wrappers.hpp>
#include <util/util.hpp>
#include <util/platform.h>
#include <platform.hpp>

OBSAbout::OBSAbout(QWidget *parent) : QDialog(parent), ui(new Ui::OBSAbout)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui->setupUi(this);

	QString bitness;
	QString ver;

	if (sizeof(void *) == 4)
		bitness = " (32 bit)";
	else if (sizeof(void *) == 8)
		bitness = " (64 bit)";

#ifdef HAVE_OBSCONFIG_H
	ver += obs_get_version_string();
#else
	ver += LIBOBS_API_MAJOR_VER + "." + LIBOBS_API_MINOR_VER + "." +
	       LIBOBS_API_PATCH_VER;
#endif

	ui->version->setText(ver + bitness);

	ui->contribute->setText("IrlosStudio — irlosd control plane for IRL streaming");

	delete ui->donate;

	ui->getInvolved->setText(
		"&nbsp;&nbsp;<a href='https://github.com/obsproject/obs-studio'>" +
		QString("OBS Studio (upstream)") + "</a>");
	ui->getInvolved->setTextInteractionFlags(Qt::TextBrowserInteraction);
	ui->getInvolved->setOpenExternalLinks(true);

	ui->about->setText("<a href='#'>" + QTStr("About") + "</a>");
	ui->authors->setText("<a href='#'>" + QTStr("About.Authors") + "</a>");
	ui->license->setText("<a href='#'>" + QTStr("About.License") + "</a>");

	ui->name->setProperty("class", "text-heading");
	ui->version->setProperty("class", "text-large");
	ui->about->setProperty("class", "bg-base");
	ui->authors->setProperty("class", "bg-base");
	ui->license->setProperty("class", "bg-base");
	ui->info->setProperty("class", "");

	connect(ui->about, &ClickableLabel::clicked, this,
		&OBSAbout::ShowAbout);
	connect(ui->authors, &ClickableLabel::clicked, this,
		&OBSAbout::ShowAuthors);
	connect(ui->license, &ClickableLabel::clicked, this,
		&OBSAbout::ShowLicense);

	ShowAbout();
}

void OBSAbout::ShowAbout()
{
	ui->textBrowser->setHtml(
		"<p>IrlosStudio is a stripped OBS Studio fork built for "
		"IRL streaming appliances, controlled via obs-websocket by "
		"irlosd.</p>"
		"<p>Based on <a href='https://github.com/obsproject/obs-studio'>"
		"OBS Studio</a> (GPL-2.0).</p>");
}

void OBSAbout::ShowAuthors()
{
	std::string path;
	QString error =
		QTStr("About.Error")
			.arg("https://github.com/obsproject/obs-studio/blob/master/AUTHORS");

#ifdef __APPLE__
	if (!GetDataFilePath("AUTHORS", path)) {
#else
	if (!GetDataFilePath("authors/AUTHORS", path)) {
#endif
		ui->textBrowser->setPlainText(error);
		return;
	}

	ui->textBrowser->setPlainText(QString::fromStdString(path));

	BPtr<char> text = os_quick_read_utf8_file(path.c_str());

	if (!text || !*text) {
		ui->textBrowser->setPlainText(error);
		return;
	}

	ui->textBrowser->setPlainText(QT_UTF8(text));
}

void OBSAbout::ShowLicense()
{
	std::string path;
	QString error =
		QTStr("About.Error")
			.arg("https://github.com/obsproject/obs-studio/blob/master/COPYING");

	if (!GetDataFilePath("license/gplv2.txt", path)) {
		ui->textBrowser->setPlainText(error);
		return;
	}

	BPtr<char> text = os_quick_read_utf8_file(path.c_str());

	if (!text || !*text) {
		ui->textBrowser->setPlainText(error);
		return;
	}

	ui->textBrowser->setPlainText(QT_UTF8(text));
}
