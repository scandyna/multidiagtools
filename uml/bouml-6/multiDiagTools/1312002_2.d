format 208

pseudostatecanvas 128002 pseudostate_ref 187778 // initial
   xyz 105.1 57.4 2000
end
statecanvas 128130 state_ref 219394 // Starting
  
  xyzwh 398.4 53.2 2000 71 43
end
statecanvas 128258 state_ref 219522 // Idle
  
  xyzwh 376.7 160.9 2000 43 43
end
statecanvas 128386 state_ref 219650 // BulkOutTransfer
  
  xyzwh 916.4 165.5 2000 140 43
end
statecanvas 128514 state_ref 219778 // BulkOutStopped
  
  xyzwh 822.8 679.3 2000 137 81
end
statecanvas 128642 state_ref 219906 // BulkInTransfer
  
  xyzwh 166.9 200.1 2000 116 43
end
statecanvas 128770 state_ref 220034 // BulkInStopped
  
  xyzwh 34.9 895.4 2000 115 43
end
statecanvas 128898 state_ref 220162 // AbortBulkOutTransfer
  
  xyzwh 870 311.5 2000 227 97
end
statecanvas 129666 state_ref 220546 // DeviceDisconnected
  
  xyzwh 164.2 51.7 2000 151 43
end
statecanvas 129922 state_ref 220674 // InitiateAbortBulkIn
  
  xyzwh 118 353 2000 216 97
end
statecanvas 130178 state_ref 220802 // CheckAbortBulkInStatus
  
  xyzwh 134.8 596.9 2000 179 43
end
note 130946 "Legend:
IABORD: InitiateAbortBulkOutResponseDecoded
CABOSRD: CheckAbortBulkOutRStatusResponseDceoded
ICRD: InitiateClearRsponseDecoded
CCSRD: CheckClearStatusResponseDecoded"
  xyzwh 602.9 31.6 2000 523 101
statecanvas 132098 state_ref 227586 // Clear
  
  xyzwh 534.9 313.8 2000 171 81
end
statecanvas 133506 state_ref 228098 // SendingUsbtmcMessage
  
  xyzwh 546 163.6 2006 179 43
end
statecanvas 135298 state_ref 228226 // ClearEndpointHalt
  
  xyzwh 199.5 905.3 2000 261 81
end
statecanvas 135682 state_ref 228354 // WaitingClearEndpointHaltResponse
  
  xyzwh 205.4 724.4 2006 249 43
end
note 136066 "Retry ?"
  xyzwh 1203.1 717.7 2000 93 67
note 136194 "Lister évènements possible selon LIBUSB-USBTMC
 - ControlTransferComplete
 - ControlTransferTimeout
 - ControlTransferError
 - ControlTransferCancelled
 - BulkOutTransferComplete
 - BulkOutTransferTimeout
 - BulkOutTransferError
 - BulkOutTransferCancelled
 - BulkInTransferComplete
 - BulkInTransferTimeout
 - BulkInTransferError
 - BulkInTransferCancelled
 - InterruptInTransferComplete
 - InterruptInTransferTimeout
 - InterruptInTransferError
 - InterruptInTransferCancelled"
  xyzwh 44.5 474 2000 430 319
statecanvas 136322 state_ref 234882 // ClearBulkOutHalt
  
  xyzwh 756.6 554 2000 131 43
end
statecanvas 137218 state_ref 235010 // TransferError
  
  xyzwh 493.2 472.6 2000 107 43
end
transitioncanvas 129026 transition_ref 239490 // <transition>
  
  from ref 128386 z 2001 label "abortTransfers" xyz 940 251 2001 to ref 128898
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 130050 transition_ref 240002 // <transition>
  
  from ref 128642 z 2001 label "synchronizationLost" xyz 165 286 2001 to ref 129922
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 133634 transition_ref 248322 // <transition>
  decenter_begin 228
  decenter_end 216
  
  from ref 133506 z 2007 label "submitBulkOutTransfer" xyz 750 163 2007 to ref 128386
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 133762 transition_ref 248450 // <transition>
  decenter_begin 794
  decenter_end 771
  
  from ref 128386 z 2007 label "bulkOutTransferDone" xyz 756 186 2007 to ref 133506
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 133890 transition_ref 248578 // <transition>
  decenter_begin 222
  decenter_end 228
  
  from ref 128258 z 2007 label "sendCommand" xyz 434 161 2007 to ref 133506
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 134018 transition_ref 248706 // <transition>
  decenter_begin 771
  decenter_end 777
  
  from ref 133506 z 2007 label "commandSent" xyz 439 185 2007 to ref 128258
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 134530 transition_ref 248962 // <transition>
  decenter_begin 412
  
  from ref 133506 z 2007 label "abortTransfers" xyz 575 250 2007 to ref 132098
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 135426 transition_ref 249218 // <transition>
  geometry HVr
  
  from ref 128514 z 2001 to point 888 942
  line 135554 z 2001 to ref 135298
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 135810 transition_ref 249346 // <transition>
  
  from ref 135298 z 2007 label "controlTransferSubmitted" xyz 255 826 2007 to ref 135682
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 135938 transition_ref 249474 // <transition>
  decenter_begin 791
  
  from ref 135682 z 2007 label "controlTransferDone" xyz 338 452 2007 to ref 128258
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 137346 transition_ref 256258 // <transition>
  
  from ref 137218 z 2001 label "resetError" xyz 438 327 2001 to ref 128258
  write_horizontally default show_definition default drawing_language default
end
transitioncanvas 137474 transition_ref 256386 // <transition>
  decenter_begin 588
  
  from ref 128386 z 2001 label "bulkOutStall" xyz 1125 348 2001 to point 1150 188
  line 138498 z 2001 to point 1153 492
  line 138370 z 2001 to ref 137218
  write_horizontally default show_definition default drawing_language default
end
end
