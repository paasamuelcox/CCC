//
//  RBLViewController.m
//  SimpleChat
//
//  Created by redbear on 14-4-8.
//  Copyright (c) 2014年 redbear. All rights reserved.
//

#import "RBLViewController.h"



@interface RBLViewController ()
{
    NSMutableArray *tableData;
}

@end

@implementation RBLViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    bleShield = [[BLE alloc] init];
    [bleShield controlSetup];
    bleShield.delegate = self;
    
    self.navigationItem.hidesBackButton = NO;
    
    activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    UIBarButtonItem *barButton = [[UIBarButtonItem alloc] initWithCustomView:activityIndicator];
    [self navigationItem].rightBarButtonItem = barButton;
    
    

    
}

-(void) connectionTimer:(NSTimer *)timer
{
    if(bleShield.peripherals.count > 0)
    {
        [bleShield connectPeripheral:[bleShield.peripherals objectAtIndex:0]];
    }
    else
    {
        [activityIndicator stopAnimating];
        self.navigationItem.leftBarButtonItem.enabled = YES;
    }
}

- (IBAction)BLEShieldScan:(id)sender
{
    if (bleShield.activePeripheral)
        if(bleShield.activePeripheral.state == CBPeripheralStateConnected)
        {
            [[bleShield CM] cancelPeripheralConnection:[bleShield activePeripheral]];
            return;
        }
    
    if (bleShield.peripherals)
        bleShield.peripherals = nil;
    
    [bleShield findBLEPeripherals:3];
    
    [NSTimer scheduledTimerWithTimeInterval:(float)3.0 target:self selector:@selector(connectionTimer:) userInfo:nil repeats:NO];
    
    [activityIndicator startAnimating];
    self.navigationItem.leftBarButtonItem.enabled = NO;
}

-(void) bleDidReceiveData:(unsigned char *)data length:(int)length
{
    NSData *d = [NSData dataWithBytes:data length:length];
    NSString *s = [[NSString alloc] initWithData:d encoding:NSUTF8StringEncoding];
   // NSLog(@"%@", s);
    
    NSArray* myArray = [s componentsSeparatedByString:@","];
    
    NSString* inMoving = [myArray objectAtIndex:0];
    NSString* inTemp = [myArray objectAtIndex:1];
    NSString* inWeight = [myArray objectAtIndex:2];

    bool parseW = NO;
    
    [inWeight intValue];
    
    CGFloat const inMin = 0;
    CGFloat const inMax = 5;
    
    CGFloat const outMin = 0;
    CGFloat const outMax = 100;
    
    CGFloat in = [inWeight intValue];
    CGFloat out = outMin + (outMax - outMin) * (in - inMin) / (inMax - inMin);

    
   // CGFloat result = (([inWeight intValue] / 5) * 0.5) + 0.5;
    
   //  NSLog(@"%@", inWeight);
   // NSLog(@"%@", inTemp);

  
    
//    PFQuery *query = [PFQuery queryWithClassName:@"CCC"];
//    
//    // Retrieve the object by id
//    [query getObjectInBackgroundWithId:@"oldTestObjectID" block:^(PFObject *pfObject, NSError *error) {
//        
//        // Now let's update it with some new data. In this case, only cheatMode and score
//        // will get sent to the cloud. playerName hasn't changed.
//        [pfObject setObject:@"rowData" forKey:@"columnName"];
//        [pfObject saveInBackground];
//    }];

//    PFObject *gameScore = [PFObject objectWithClassName:@"GameScore"];
//    [gameScore setObject:[NSNumber numberWithBool:NO] forKey:@"cheatMode"];

    temp.text = inTemp;
    
   

    if ([inMoving  isEqual: @"1"]){
        moving.text = @"moving";
        parseW = YES;
    }
    else {
        moving.text = @"not moving";
        parseW = NO;

    }
    
    NSString* FinalWeight = [NSString stringWithFormat:@"%f", out];

    
    if(weight < 0){
        weight.text = @"0";
    }
    
    weight.text = FinalWeight;
    
    PFObject *CCC = [PFObject objectWithClassName:@"CCC"];
    CCC[@"temp"] = inTemp;
    CCC[@"weight"] = FinalWeight; //inWeight
    CCC[@"moving"] = inMoving;
    [CCC saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
        if (succeeded) {
            // The object has been saved.
        } else {
            // There was a problem, check error.description
        }
    }];

}

NSTimer *rssiTimer;

-(void) readRSSITimer:(NSTimer *)timer
{
    [bleShield readRSSI];
}

- (void) bleDidDisconnect
{
    NSLog(@"bleDidDisconnect");
    
    [self.navigationItem.leftBarButtonItem setTitle:@"Connect"];
    [activityIndicator stopAnimating];
    self.navigationItem.leftBarButtonItem.enabled = YES;
    
    [[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from:nil forEvent:nil];
}

-(void) bleDidConnect
{
    [activityIndicator stopAnimating];
    self.navigationItem.leftBarButtonItem.enabled = YES;
    [self.navigationItem.leftBarButtonItem setTitle:@"Disconnect"];
    
    NSLog(@"bleDidConnect");
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}





@end
