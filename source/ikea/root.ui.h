/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "ikea.h"

void root::AddSite()
{
	site s( this );
	if( s.exec() == QDialog::Accepted )
	{
		// save site config

		CONFIG config;
		s.Save( config );

		char path[ 1024 ];
		snprintf( path, 1024, "%s/%s",
			ikea.site_path(),
			s.lineEditHost->text().ascii() );

		config.file_write( path );

		// create icon for site

		QIconViewItem * i = new QIconViewItem( iconViewSites );
		if( i == NULL )
			return;
	
		i->setPixmap( QPixmap::fromMimeSource( "site.png" ) );
		i->setSelected( true );
		i->setText( s.lineEditHost->text() );
		i->setRenameEnabled( true );
		i->rename();
	}
}

void root::ModSite()
{
	QIconViewItem * i = iconViewSites->currentItem();
	if( i != NULL )
	{
		// load site config

		CONFIG config;

		char path[ 1024 ];
		snprintf( path, 1024, "%s/%s",
			ikea.site_path(),
			i->text().ascii() );

		config.file_read( path );

		// create site modal dialog

		site s( this );
		s.Load( config );

		if( s.exec() == QDialog::Accepted )
		{
			// save the modifications

			s.Save( config );
			config.file_write( path );
		}
	}
}

void root::DelSite()
{
	QIconViewItem * i = iconViewSites->currentItem();
	if( i != NULL )
	{
		QMessageBox m;
		if( m.warning( this,
			   "Warning",
			   "Are you sure you want to delete this site?",
			   QMessageBox::Yes,
			   QMessageBox::Cancel ) != QMessageBox::Yes )
			return;

		char path[ 1024 ];
		snprintf( path, 1024, "%s/%s",
			ikea.site_path(),
			i->text().ascii() );

		unlink( path );
		delete i;
	}
}


void root::ContextSite( QIconViewItem * item, const QPoint & pos )
{
	QPopupMenu m;
	m.insertItem( QPixmap::fromMimeSource( "site_con.png" ), "Connect", 0 );
	m.insertSeparator();
	m.insertItem( QPixmap::fromMimeSource( "site_add.png" ), "Add", 1 );
	m.insertItem( QPixmap::fromMimeSource( "site_del.png" ), "Delete", 2 );
	m.insertItem( "Rename", 3 );
	m.insertSeparator();
	m.insertItem( QPixmap::fromMimeSource( "site_mod.png" ), "Properties", 4 );

	if( item == NULL )
	{
		m.setItemEnabled( 0, false );
		m.setItemEnabled( 2, false );
		m.setItemEnabled( 3, false );
		m.setItemEnabled( 4, false );
	}

	int result = m.exec( pos );
	switch( result )
	{
		case 0:
			break;

		case 1:
			AddSite();
			break;

		case 2:
			DelSite();
			break;

		case 3:
			item->rename();
			break;

		case 4:
			ModSite();
			break;
	}
}


void root::About()
{
	about a( this );
	a.exec();
}