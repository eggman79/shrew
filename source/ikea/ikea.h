
/*
 * Copyright (c) 2007
 *      Shrew Soft Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Redistributions in any form must be accompanied by information on
 *    how to obtain complete source code for the software and any
 *    accompanying software that uses the software.  The source code
 *    must either be included in the distribution or be available for no
 *    more than the cost of distribution plus a nominal fee, and must be
 *    freely redistributable under reasonable conditions.  For an
 *    executable file, complete source code means the source code for all
 *    modules it contains.  It does not include source code for modules or
 *    files that typically accompany the major components of the operating
 *    system on which the executable file runs.
 *
 * THIS SOFTWARE IS PROVIDED BY SHREW SOFT INC ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
 * NON-INFRINGEMENT, ARE DISCLAIMED.  IN NO EVENT SHALL SHREW SOFT INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * AUTHOR : Matthew Grooms
 *          mgrooms@shrew.net
 *
 */

#ifndef _IKEA_H_
#define _IKEA_H_

#include <pwd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDir>

#include "../version.h"
#include "config.h"
#include "ui_root.h"
#include "ui_site.h"
#include "ui_conflict.h"
#include "ui_topology.h"
#include "ui_about.h"

#define	CONFLICT_OVERWRITE	100
#define	CONFLICT_CONTINUE	101

typedef class _ikeaRoot : public QMainWindow, public Ui::ikeaRoot
{
	Q_OBJECT

	QActionGroup *	actionGroupView;
	QMenu *		menuContext;
	QMenu *		menuContextView;

	public:

	_ikeaRoot( QWidget * parent = NULL ) : QMainWindow( parent )
	{
		setupUi( this );

		actionGroupView = new QActionGroup( this );
		actionGroupView->addAction( actionViewLarge );
		actionGroupView->addAction( actionViewSmall );

		menuView->addAction( toolBar->toggleViewAction() );

		menuContext = new QMenu( listWidgetSites );
		menuContextView = new QMenu( "View", listWidgetSites );

		connect( listWidgetSites, SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( siteContext( const QPoint & ) ) );
		connect( listWidgetSites, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( siteRenamed( QListWidgetItem * ) ) );
		connect( listWidgetSites, SIGNAL( itemDoubleClicked ( QListWidgetItem * ) ), this, SLOT( siteConnect() ) ); 

		connect( actionConnect, SIGNAL( triggered() ), this, SLOT( siteConnect() ) );
		connect( actionAdd, SIGNAL( triggered() ), this, SLOT( siteAdd() ) );
		connect( actionModify, SIGNAL( triggered() ), this, SLOT( siteModify() ) );
		connect( actionDelete, SIGNAL( triggered() ), this, SLOT( siteDelete() ) );
		connect( actionRename, SIGNAL( triggered() ), this, SLOT( siteRename() ) );

		connect( actionViewLarge, SIGNAL( triggered() ), this, SLOT( showViewLarge() ) );
		connect( actionViewSmall, SIGNAL( triggered() ), this, SLOT( showViewSmall() ) );

		connect( actionImport, SIGNAL( triggered() ), this, SLOT( siteImport() ) );
		connect( actionExport, SIGNAL( triggered() ), this, SLOT( siteExport() ) );

		connect( actionAbout, SIGNAL( triggered() ), this, SLOT( showAbout() ) );
		connect( actionExit, SIGNAL( triggered() ), this, SLOT( close() ) );
	}

	void fileConflict( QString & path, QString & name );

	private slots:

	void showViewLarge();
	void showViewSmall();

	void siteContext( const QPoint & pos );

	void siteConnect();
	void siteAdd();
	void siteModify();
	void siteDelete();
	void siteRename();
	void siteRenamed( QListWidgetItem * );

	void siteImport();
	void siteExport();

	void showAbout();

}ikeaRoot;

typedef class _ikeaSite : public QDialog, public Ui::ikeaSite
{
	Q_OBJECT

	public:
	
	_ikeaSite( QWidget * parent = NULL ) : QDialog( parent )
	{
		setupUi( this );
		init();

		connect( buttonSave, SIGNAL( clicked() ), this, SLOT( verify() ) );
		connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

		connect( checkBoxAddressAuto, SIGNAL( clicked() ), this, SLOT( updateAddressAuto() ) );

		connect( checkBoxDNSEnable, SIGNAL( clicked() ), this, SLOT( updateNameResolution() ) );
		connect( checkBoxDNSAuto, SIGNAL( clicked() ), this, SLOT( updateNameResolution() ) );
		connect( checkBoxSuffixAuto, SIGNAL( clicked() ), this, SLOT( updateNameResolution() ) );

		connect( checkBoxLocalIDOption, SIGNAL( clicked() ), this, SLOT( updateLocalID() ) );
		connect( checkBoxRemoteIDOption, SIGNAL( clicked() ), this, SLOT( updateRemoteID() ) );

		connect( checkBoxPolicyAuto, SIGNAL( clicked() ), this, SLOT( updatePolicy() ) );

		connect( comboBoxAuthMethod, SIGNAL( activated( int ) ), this, SLOT( updateAuthentication() ) );
		connect( comboBoxConfigMethod, SIGNAL( activated( int ) ), this, SLOT( updateConfigMethod() ) );
		connect( comboBoxAddressMethod, SIGNAL( activated( int ) ), this, SLOT( updateAddressMethod() ) );

		connect( comboBoxFragMode, SIGNAL( activated( int ) ), this, SLOT( updateClient() ) );
		connect( comboBoxNATTMode, SIGNAL( activated( int ) ), this, SLOT( updateClient() ) );

		connect( comboBoxLocalIDType, SIGNAL( activated( int ) ), this, SLOT( updateLocalID() ) );
		connect( comboBoxRemoteIDType, SIGNAL( activated( int ) ), this, SLOT( updateRemoteID() ) );

		connect( comboBoxP1Exchange, SIGNAL( activated( int ) ), this, SLOT( updatePhase1() ) );
		connect( comboBoxP1Cipher, SIGNAL( activated( int ) ), this, SLOT( updatePhase1() ) );

		connect( comboBoxP2Transform, SIGNAL( activated( int ) ), this, SLOT( updatePhase2() ) );

		connect( pushButtonPolicyAdd, SIGNAL( clicked() ), this, SLOT( policyAdd() ) );
		connect( pushButtonPolicyMod, SIGNAL( clicked() ), this, SLOT( policyModify() ) );
		connect( pushButtonPolicyDel, SIGNAL( clicked() ), this, SLOT( policyDelete() ) );

		connect( toolButtonCAFile, SIGNAL( clicked() ), this, SLOT( inputCAFile() ) );
		connect( toolButtonCertFile, SIGNAL( clicked() ), this, SLOT( inputCertFile() ) );
		connect( toolButtonPKeyFile, SIGNAL( clicked() ), this, SLOT( inputPKeyFile() ) );

		connect( treeWidgetPolicies, SIGNAL( itemSelectionChanged() ), this, SLOT( updatePolicy() ) );
	}

	void init();
	bool load( CONFIG & config );
	bool save( CONFIG & config );

	private slots:

	bool verify();

	void policyAdd();
	void policyModify();
	void policyDelete();

	void updateConfigMethod();
	void updateAddressAuto();
	void updateAddressMethod();
	void updateGeneral( bool adflt, bool mdflt );
	void updateClient();
	void updateNameResolution();
	void updateAuthentication();
	void updateLocalID();
	void updateRemoteID();
	void updatePhase1();
	void updatePhase2();
	void updatePolicy();

	void selectLocalID();
	void selectRemoteID();

	void inputCAFile();
	void inputCertFile();
	void inputPKeyFile();

}ikeaSite;

typedef class _ikeaConflict : public QDialog, public Ui::ikeaConflict
{
	Q_OBJECT

	public:
	
	_ikeaConflict( QWidget * parent = NULL ) : QDialog( parent )
	{
		setupUi( this );

		connect( buttonContinue, SIGNAL( clicked() ), this, SLOT( accept() ) );
		connect( buttonOverwrite, SIGNAL( clicked() ), this, SLOT( reject() ) );
	}

	private slots:

}ikeaConflict;

typedef class _ikeaTopology : public QDialog, public Ui::ikeaTopology
{
	Q_OBJECT

	public:
	
	_ikeaTopology( QWidget * parent = NULL ) : QDialog( parent )
	{
		setupUi( this );

		connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
		connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	}

	private slots:

}ikeaTopology;

typedef class _ikeaAbout : public QDialog, public Ui::ikeaAbout
{
	Q_OBJECT

	public:
	
	_ikeaAbout( QWidget * parent = NULL ) : QDialog( parent )
	{
		setupUi( this );

		connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	}

	private slots:

}ikeaAbout;

typedef class _IKEA
{
	protected:

	ikeaRoot * r;

	public:

	QString sites;
	QString certs;

	_IKEA();
	~_IKEA();

	const char * site_path();
	const char * cert_path();

	bool init( ikeaRoot * setRoot );

}IKEA;

extern IKEA ikea;

void update_site( CONFIG * config, const char * path, long & version );

#endif
