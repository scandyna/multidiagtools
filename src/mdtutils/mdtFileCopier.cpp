/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtFileCopier.h"
#include "mdtFileCopierItem.h"
#include "mdtError.h"
#include <QMutableLinkedListIterator>
#include <QLabel>
#include <QCryptographicHash>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#ifdef Q_OS_WIN
 #include <windows.h>
#endif  // #ifdef Q_OS_WIN
#include <stdio.h>


#include <QDebug>

mdtFileCopier::mdtFileCopier(QObject *parent)
 : QThread(parent)
{
  pvBufferSize = BUFSIZ;
  pvBuffer = new char[pvBufferSize];
  Q_ASSERT(pvBuffer != 0);
  pvFinished = true;
  // Set the default setup
  setDefaultSetup();
  // Progress dialog
  pvProgressDialog = new QProgressDialog;
  // Message box: make it child of progress dialog, so it shows allways on top
  pvMessageBox = new QMessageBox(pvProgressDialog);
  // We need a custom label, because we want to set alignement to left
  QLabel *label = new QLabel;
  label->setAlignment(Qt::AlignLeft);
  pvProgressDialog->setLabel(label);  // Note: QProgressDialog will delete label itself
  pvProgressDialog->setWindowModality(Qt::WindowModal);
  connect(this, SIGNAL(newProgressValue(int)), pvProgressDialog, SLOT(setValue(int)));
  connect(this, SIGNAL(newProgressRange(int, int)), pvProgressDialog, SLOT(setRange(int, int)));
  connect(this, SIGNAL(finished(bool)), pvProgressDialog, SLOT(reset()));
  connect(this, SIGNAL(newProgressLabelText(const QString&)), pvProgressDialog, SLOT(setLabelText(const QString&)));
  connect(pvProgressDialog, SIGNAL(canceled()), this, SLOT(cancelCopy()));
  // Thread signals
  connect(this, SIGNAL(copyEnded(const QString&, const QString&)), this, SLOT(displayCopyResult(const QString&, const QString&)));
}

mdtFileCopier::~mdtFileCopier()
{
  wait();
  qDeleteAll(pvCopiesInProcess);
  delete[] pvBuffer;
  delete pvMessageBox;
  delete pvProgressDialog;
  qDeleteAll(pvCopies);
}

void mdtFileCopier::setShowProgressDialog(bool show)
{
  pvMutex.lock();
  pvShowProgressDialog = show;
  pvMutex.unlock();
}

void mdtFileCopier::setDirectDestOverwrite(bool directOverwrite)
{
  pvMutex.lock();
  pvDirectDestOverwrite = directOverwrite;
  pvMutex.unlock();
}

void mdtFileCopier::diseableAllDialogs(bool diseable)
{
  pvMutex.lock();
  pvMutex.unlock();
}

void mdtFileCopier::setDefaultSetup()
{
  pvMutex.lock();
  pvShowProgressDialog = true;
  pvDirectDestOverwrite = false;
  pvAllDialogsDiseabled = false;
  pvMutex.unlock();
}

void mdtFileCopier::addCopy(const QString & srcFilePath, const QString & destFilePath, bool syncAfterCopy, bool checkAfterCopy)
{
  mdtFileCopierItem *item = new mdtFileCopierItem(srcFilePath, destFilePath);
  Q_ASSERT(item != 0);
  item->setSyncAfterCopy(syncAfterCopy);
  item->setCheckAfterCopy(checkAfterCopy);
  pvCopies << item;
}

void mdtFileCopier::startCopy()
{
  QString msgDetails;
  mdtFileCopierItem *item;
  bool applyOverwrite = false;
  bool askBeforeOverwrite = true; // Ask the user before overwrite existing destination. Becomes false when user selected YesForAll or NoForAll
  int retVal;
  QMutableLinkedListIterator<mdtFileCopierItem*> it(pvCopies);
  FILE *destTestFile;

  while(it.hasNext()){
    item = it.next();
    Q_ASSERT(item != 0);
    // Set file infos
    pvSrcFileInfo.setFile(item->srcFilePath());
    pvDestFileInfo.setFile(item->destFilePath());
    // Check if source files exists
    if(!pvSrcFileInfo.exists()){
      msgDetails += tr("Source file '") + pvSrcFileInfo.fileName() + tr("' was not found in '") + pvSrcFileInfo.absolutePath() + "'\n\n";
      it.remove();
      continue;
    }
    // Check if source is a directory
    if(pvSrcFileInfo.isDir()){
      msgDetails += tr("Source '") + pvSrcFileInfo.fileName() + tr("' is a directory\n\n");
      it.remove();
      continue;
    }
    // Check if file exists on destination
    if(pvDestFileInfo.isDir()){
      // Rebuild destination with dir/srcFileName
      item->setDestFilePath(pvDestFileInfo.absoluteFilePath() + "/" + pvSrcFileInfo.fileName());
      pvDestFileInfo.setFile(item->destFilePath());
    }
    if(pvDestFileInfo.exists()){
      if(askBeforeOverwrite){
        // Ask the user what to do, and give a option to apply choice to all (next) files
        pvMessageBox->setText(tr("File '") + pvDestFileInfo.fileName() + tr("' allready exists. Overwrite it ?"));
        pvMessageBox->setInformativeText(tr("Destination: ") + pvDestFileInfo.absolutePath());
        pvMessageBox->setIcon(QMessageBox::Question);
        pvMessageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
        pvMessageBox->setDefaultButton(QMessageBox::No);
        retVal = pvMessageBox->exec();
        // Get user's choice
        switch(retVal){
          case QMessageBox::YesToAll:
            applyOverwrite = true;
            askBeforeOverwrite = false;
            break;
          case QMessageBox::Yes:
            applyOverwrite = false;
            askBeforeOverwrite = true;
            break;
          case QMessageBox::NoToAll:
            applyOverwrite = false;
            askBeforeOverwrite = false;
            it.remove();
            continue;
          default:  // No, Cancel, ...
            applyOverwrite = false;
            askBeforeOverwrite = true;
            it.remove();
            continue;
        }
      }else{
        // User choosed YesToAll or NoToAll
        if(!applyOverwrite){
          it.remove();
          continue;
        }
      }
    }
    // Set source file name
    item->setSrcFileName(pvSrcFileInfo.fileName());
    // Set absolute file path to source and destination
    item->setSrcFilePath(pvSrcFileInfo.absoluteFilePath());
    item->setDestFilePath(pvDestFileInfo.absoluteFilePath());
    // Here we are shure that destination can be written, try it now
    destTestFile = fopen(item->destFilePath().toAscii().data(), "w");
    if(destTestFile == 0){
      switch(errno){
        case EACCES:
          msgDetails += tr("Cannot copy file '") + pvSrcFileInfo.fileName() + tr("' to '") + pvDestFileInfo.absolutePath() + tr("' : access denied\n\n");
          break;
        case ENOENT:
          msgDetails += tr("Cannot copy file '") + pvSrcFileInfo.fileName() + tr("' to '") + pvDestFileInfo.absolutePath() + tr("' : destination not found\n\n");
          break;
        case EROFS:
          msgDetails += tr("Cannot copy file '") + pvSrcFileInfo.fileName() + tr("' to '") + pvDestFileInfo.absolutePath() + tr("' : filesystem is read only\n\n");
          break;
        default:
          mdtError e(MDT_FILE_IO_ERROR, "fopen() failed for " + item->destFilePath(), mdtError::Error);
          e.setSystemError(errno, strerror(errno));
          MDT_ERROR_SET_SRC(e, "mdtFileCopier");
          e.commit();
          msgDetails += tr("Cannot copy file '") + pvSrcFileInfo.fileName() + tr("' to '") + pvDestFileInfo.absolutePath() + " : " + QString(strerror(errno)) + "\n";
          break;
      }
      it.remove();
      continue;
    }
    fclose(destTestFile);
    // NOTE: remove destination file here !
    
    // Set copy size
    item->setFileSize(pvSrcFileInfo.size());
  }
  // If no file can be copied, tell this the user
  if(pvCopies.size() < 1){
    pvMessageBox->setText(tr("No copy can be done"));
    if(msgDetails.size() > 0){
      pvMessageBox->setInformativeText(tr("More informations are available in details"));
      pvMessageBox->setDetailedText(msgDetails);
    }else{
      pvMessageBox->setInformativeText(tr(""));
      pvMessageBox->setDetailedText("");
    }
    pvMessageBox->setIcon(QMessageBox::Warning);
    pvMessageBox->setStandardButtons(QMessageBox::Ok);
    pvMessageBox->exec();
    return;
  }
  // If some messages where collected, show to user, and ask if copy must be done
  if(!msgDetails.isEmpty()){
    pvMessageBox->setText(tr("Some copies cannot be done, do you want to continue ?"));
    pvMessageBox->setInformativeText(tr("More informations are available in details"));
    pvMessageBox->setDetailedText(msgDetails);
    pvMessageBox->setIcon(QMessageBox::Warning);
    pvMessageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::Abort);
    pvMessageBox->setDefaultButton(QMessageBox::Abort);
    retVal = pvMessageBox->exec();
    if(retVal != QMessageBox::Yes){
      return;
    }
  }
  // Add possible copies to the process list
  it.toFront();
  pvMutex.lock();
  while(it.hasNext()){
    item = it.next();
    Q_ASSERT(item != 0);
    it.remove();
    pvCopiesInProcess.append(item);
  }
  pvFinished = false;
  pvCancelCopy = false;
  pvMutex.unlock();
  // Start thread if needed
  if(!isRunning()){
    start();
  }

  ///pvProgressDialog->setRange(0, 100);
  pvProgressDialog->setLabelText(tr("\nBegin copy ..."));

  pvProgressDialog->exec();
}

bool mdtFileCopier::waitFinished()
{
  pvMutex.lock();
  pvMutex.unlock();
}

void mdtFileCopier::cancelCopy()
{
  pvMutex.lock();
  pvCancelCopy = true;
  pvMutex.unlock();
  pvProgressDialog->reset();
  // Wait end of thread and display message
  wait();
  pvMessageBox->setText(tr("Copy aborted"));
  pvMessageBox->setInformativeText("");
  pvMessageBox->setDetailedText("");
  pvMessageBox->setIcon(QMessageBox::Warning);
  pvMessageBox->setStandardButtons(QMessageBox::Ok);
  pvMessageBox->exec();
}

void mdtFileCopier::displayCopyResult(const QString &failedCopies, const QString &informativeText)
{
  if(failedCopies.size() > 0){
    pvMessageBox->setText(tr("Some copies failed                                                "));  // Don't want this too little message box..
    if(informativeText.size() > 0){
      pvMessageBox->setInformativeText(tr("The list of failed copies are available in details\n") + informativeText);
    }else{
      pvMessageBox->setInformativeText(tr("The list of failed copies are available in details"));
    }
    pvMessageBox->setDetailedText(failedCopies);
    pvMessageBox->setIcon(QMessageBox::Warning);
    pvMessageBox->setStandardButtons(QMessageBox::Ok);
    pvMessageBox->exec();
  }else{
    pvMessageBox->setText(tr("All copies are done"));
    pvMessageBox->setInformativeText(informativeText);
    pvMessageBox->setDetailedText("");
    pvMessageBox->setIcon(QMessageBox::Information);
    pvMessageBox->setStandardButtons(QMessageBox::Ok);
    pvMessageBox->exec();
  }
}

bool mdtFileCopier::sync(FILE *f/*, const QString &Filepath*/)
{
  Q_ASSERT(f != 0);

#ifdef Q_OS_WIN
  HANDLE h = (HANDLE)_get_osfhandle(fileno(f));
  if(h == INVALID_HANDLE_VALUE){
    mdtError e(MDT_FILE_IO_ERROR, "Getting file handle failed", mdtError::Error);
    e.setSystemErrorWinApi();
    MDT_ERROR_SET_SRC(e, "mdtFileCopier");
    e.commit();
    return false;
  }
  if(FlushFileBuffers(h) == 0){
    mdtError e(MDT_FILE_IO_ERROR, "FlushFileBuffers call failed", mdtError::Error);
    e.setSystemErrorWinApi();
    MDT_ERROR_SET_SRC(e, "mdtFileCopier");
    e.commit();
    return false;
  }
#else
  if(fsync(fileno(f)) < 0){
    mdtError e(MDT_FILE_IO_ERROR, "Sync call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtFileCopier");
    e.commit();
    return false;
  }
#endif

  return true;
}

void mdtFileCopier::run()
{
  FILE *srcFile;
  FILE *destFile;
  size_t readen;
  size_t written;
  size_t charSize = sizeof(char);
  mdtFileCopierItem *item;
  qint64 copySize = 0;
  qint64 copyProgress = 0;
  qint64 copySizeDivisor = 1;
  int previousCopiesCount = 0;
  int i;
  QString failedCopies = "";
  QString copyInfos;
  QCryptographicHash hash(QCryptographicHash::Sha1);  // To calculate file "sum"

  // Copy all available copies
  while(1){
    pvMutex.lock();
    // Check if somthing is to copy
    if(pvCopiesInProcess.size() < 1){
      pvMutex.unlock();
      break;
    }
    // See if we have to (re-)calculate copy size
    if(pvCopiesInProcess.size() > previousCopiesCount){
      for(i=0; i<pvCopiesInProcess.size(); i++){
        item = pvCopiesInProcess.at(i);
        Q_ASSERT(item != 0);
        copySize += item->fileSize();
      }
      previousCopiesCount = pvCopiesInProcess.size();
      // Select divisor regarding total size, and set range
      Q_ASSERT(copySize >= Q_INT64_C(0));
      copySizeDivisor = 1;
      i=0;
      while( copySizeDivisor < (copySize / Q_INT64_C(10000)) ){
        copySizeDivisor = (1<<i);
        i++;
      }
      Q_ASSERT((copySize/copySizeDivisor) <= Q_INT64_C(10000) );
      Q_ASSERT((copySize/copySizeDivisor) >= Q_INT64_C(0) );
      qDebug() << "Copy size: " << copySize << " , divisor: " << copySizeDivisor << " , copySize/copySizeDivisor: " << copySize/copySizeDivisor;
      emit(newProgressRange(0, (int)(copySize/copySizeDivisor)));
    }
    // Take item and unlock mutex
    item = pvCopiesInProcess.takeFirst();
    pvMutex.unlock();
    Q_ASSERT(item != 0);
    // Some infos to display during copy ...
    if(item->syncAfterCopy()){
      copyInfos = tr("\n\nSync after copy: on");
    }else{
      copyInfos = tr("\n\nSync after copy: off");
    }
    if(item->checkAfterCopy()){
      copyInfos += tr("      Verify after copy: on");
    }else{
      copyInfos += tr("      Verify after copy: off");
    }
    // Open source and destination files
    emit(newProgressLabelText(tr("\nCopying file ") + item->srcFileName() + tr("\n to ") + item->destFilePath() + copyInfos));
    srcFile = fopen(item->srcFilePath().toAscii().data(), "rb");
    if(srcFile == 0){
      mdtError e(MDT_FILE_IO_ERROR, "Cannot open file: " + item->srcFilePath(), mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtFileCopier");
      e.commit();
      failedCopies += item->srcFileName() + " -> " + item->destFilePath() + "\n\n";
      continue;
    }
    destFile = fopen(item->destFilePath().toAscii().data(), "wb");
    if(destFile == 0){
      mdtError e(MDT_FILE_IO_ERROR, "Cannot open file: " + item->destFilePath(), mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtFileCopier");
      e.commit();
      fclose(srcFile);
      failedCopies += item->srcFileName() + " -> " + item->destFilePath() + "\n\n";
      continue;
    }
    // Reset hash
    hash.reset();
    // Copy ..
    do{
      // See if copy must be aborted
      pvMutex.lock();
      if(pvCancelCopy){
        // Close opened files
        fclose(srcFile);
        fclose(destFile);
        // Remove current destination file (avoid currupted files..)
        if(remove(item->destFilePath().toAscii().data()) < 0){
          mdtError e(MDT_FILE_IO_ERROR, "Cannot remove file: " + item->destFilePath(), mdtError::Error);
          e.setSystemError(errno, strerror(errno));
          MDT_ERROR_SET_SRC(e, "mdtFileCopier");
          e.commit();
        }
        pvMutex.unlock();
        return;
      }
      pvMutex.unlock();
      // Process copy ...
      readen = fread(pvBuffer, charSize, pvBufferSize, srcFile);
      // Hash, if needed
      if(item->checkAfterCopy()){
        hash.addData(pvBuffer, readen);
      }
      // Write to destination
      written = fwrite(pvBuffer, charSize, readen, destFile);
      if(written < readen){
        // Write call failed, this is fatal, cancel the copy
        fclose(srcFile);
        fclose(destFile);
        failedCopies += item->srcFileName() + " -> " + item->destFilePath() + "\n\n";
        switch(errno){
          case ENOSPC:  // Destination device full
            emit(finished(false));
            emit(copyEnded(failedCopies, tr("Note: destination device is full")));
            return;
          default:
            mdtError e(MDT_FILE_IO_ERROR, "Write call failed on file: " + item->destFilePath(), mdtError::Error);
            e.setSystemError(errno, strerror(errno));
            MDT_ERROR_SET_SRC(e, "mdtFileCopier");
            e.commit();
            return;
        }
      }
      copyProgress += (qint64)written;
      emit(newProgressValue((int)(copyProgress/copySizeDivisor)));
    }while(readen > 0);
    // Hash, if needed
    if(item->checkAfterCopy()){
      item->setSrcFileHash(hash.result());
    }
    // Flush
    if(fflush(destFile) != 0){
      mdtError e(MDT_FILE_IO_ERROR, "Cannot flush file: " + item->destFilePath(), mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtFileCopier");
      e.commit();
      failedCopies += item->srcFileName() + " -> " + item->destFilePath() + "\n\n";
    }
    // Sync, if needed
    if(item->syncAfterCopy()){
      if(!sync(destFile)){
        // Sync fail is fatal (device full, not available, ...) - cancel copy
        failedCopies += item->srcFileName() + " -> " + item->destFilePath() + "\n\n";
        fclose(srcFile);
        fclose(destFile);
        emit(finished(false));
        emit(copyEnded(failedCopies));
        return;
      }
    }
    // Close opened files
    if(fclose(srcFile) != 0){
      mdtError e(MDT_FILE_IO_ERROR, "Cannot close file: " + item->srcFilePath(), mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtFileCopier");
      e.commit();
    }
    srcFile = 0;
    if(fclose(destFile) != 0){
      mdtError e(MDT_FILE_IO_ERROR, "Cannot close file: " + item->destFilePath(), mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtFileCopier");
      e.commit();
      failedCopies += item->srcFileName() + " -> " + item->destFilePath() + "\n\n";
    }
    destFile = 0;
    // Check destination (with hash) if needed
    if(item->checkAfterCopy()){
      // Open destination read only
      destFile = fopen(item->destFilePath().toAscii().data(), "r");
      if(destFile == 0){
        mdtError e(MDT_FILE_IO_ERROR, "Cannot open file: " + item->destFilePath(), mdtError::Error);
        e.setSystemError(errno, strerror(errno));
        MDT_ERROR_SET_SRC(e, "mdtFileCopier");
        e.commit();
        failedCopies += item->srcFileName() + " -> " + item->destFilePath() + tr("\n Open for verification failed\n\n");
        continue;
      }
      // Read and calculate hash
      hash.reset();
      do{
        readen = fread(pvBuffer, charSize, pvBufferSize, destFile);
        hash.addData(pvBuffer, readen);
      }while(readen > 0);
      fclose(destFile);
      item->setDestFileHash(hash.result());
      // Compare results
      ///qDebug() << "File " << item->srcFileName() << ": SHA-1, SRC: " << item->srcFileHash().toHex() << " , DST: " << item->destFileHash().toHex();
      if(item->destFileHash() != item->srcFileHash()){
        failedCopies += item->srcFileName() + " -> " + item->destFilePath() + tr("\n Hash verification failed\n\n");
      }
    }
  }
  // Update finished state
  pvMutex.lock();
  pvFinished = true;
  pvMutex.unlock();
  if(!pvCancelCopy){
    emit(finished(true));
    emit(copyEnded(failedCopies));
  }
}

