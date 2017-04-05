//
//  RBLViewController.h
//  SimpleChat
//
//  Created by redbear on 14-4-8.
//  Copyright (c) 2014年 redbear. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BLE.h"
#import <Parse/Parse.h>

@interface RBLViewController : UIViewController <UITableViewDataSource, UITableViewDelegate, UITextFieldDelegate, BLEDelegate>
{
    BLE *bleShield;
    UIActivityIndicatorView *activityIndicator;
    IBOutlet UILabel *weight;
    IBOutlet UILabel *moving;
    IBOutlet UILabel *temp;
}



@end
