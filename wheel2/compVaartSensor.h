#define sampleMax 65500               //samples



class COMPVAART{
  public:
    volatile unsigned int vaartInterval;
    volatile unsigned int sampleNum;
    volatile          int samples[100];
    volatile unsigned int sampleTeller = 0;
    volatile unsigned long tijd;
    volatile unsigned int interval;
    float vaart;
    float gemiddelde = sampleMax;

    bool Anu;
    bool Aoud;
    int dir;

    float pulsenPerRev;
    int teller = 0;
    int pin;

    float compensatie[600];
    // float compensatieAct[600] = {1.0076, 1.0067, 1.0090, 1.0103, 1.0062, 1.0049, 1.0071, 1.0076, 1.0060, 1.0105, 1.0051, 1.0069, 1.0068, 1.0059, 1.0069, 1.0053, 1.0078, 1.0028, 1.0061, 1.0044, 1.0049, 1.0002, 1.0058, 1.0036, 1.0023, 0.9998, 1.0005, 0.9975, 1.0030, 1.0013, 1.0010, 0.9942, 0.9943, 0.9980, 0.9955, 0.9964, 0.9933, 0.9964, 0.9914, 0.9916, 0.9885, 0.9902, 0.9907, 0.9934, 0.9900, 0.9898, 0.9869, 0.9861, 0.9868, 0.9819, 0.9903, 0.9870, 0.9874, 0.9865, 0.9876, 0.9904, 0.9847, 0.9906, 0.9868, 0.9874, 0.9862, 0.9877, 0.9876, 0.9870, 0.9909, 0.9882, 0.9903, 0.9860, 0.9906, 0.9888, 0.9850, 0.9891, 0.9898, 0.9905, 0.9939, 0.9938, 0.9935, 0.9948, 1.0006, 1.0015, 0.9991, 1.0020, 1.0025, 0.9977, 1.0035, 0.9999, 1.0044, 1.0051, 1.0058, 1.0081, 1.0069, 1.0073, 1.0124, 1.0128, 1.0114, 1.0119, 1.0125, 1.0100, 1.0162, 1.0151, 1.0133, 1.0194, 1.0167, 1.0156, 1.0204, 1.0207, 1.0191, 1.0160, 1.0235, 1.0181, 1.0146, 1.0196, 1.0198, 1.0219, 1.0180, 1.0220, 1.0177, 1.0185, 1.0226, 1.0256, 1.0162, 1.0176, 1.0153, 1.0150, 1.0176, 1.0122, 1.0156, 1.0142, 1.0126, 1.0098, 1.0112, 1.0065, 1.0062, 1.0047, 1.0055, 1.0052, 1.0035, 1.0033, 1.0060, 1.0023, 1.0032, 1.0014, 0.9981, 0.9941, 0.9970, 0.9933, 0.9990, 0.9981, 0.9982, 0.9980, 0.9945, 0.9940, 0.9911, 0.9930, 0.9933, 0.9946, 0.9925, 0.9919, 0.9937, 0.9885, 0.9913, 0.9888, 0.9930, 0.9904, 0.9873, 0.9894, 0.9943, 0.9884, 0.9918, 0.9936, 0.9925, 0.9924, 0.9927, 0.9915, 0.9910, 0.9939, 0.9932, 0.9941, 0.9922, 0.9969, 0.9954, 0.9963, 0.9956, 0.9948, 0.9970, 0.9978, 0.9969, 0.9998, 1.0004, 0.9963, 0.9956, 1.0003, 0.9980, 0.9969, 1.0064, 1.0036, 1.0000, 1.0041, 1.0015, 1.0055, 1.0057, 1.0042, 1.0059, 1.0023, 1.0045, 1.0073, 1.0063, 1.0071, 1.0082, 1.0063, 1.0048, 1.0034, 1.0064, 1.0058, 1.0068, 1.0035, 1.0050, 1.0044, 1.0074, 1.0060, 1.0038, 1.0075, 1.0049, 1.0023, 1.0044, 1.0001, 1.0029, 1.0046, 0.9997, 0.9998, 0.9963, 1.0013, 0.9985, 0.9991, 0.9978, 0.9993, 0.9957, 0.9904, 0.9973, 0.9929, 0.9923, 0.9931, 0.9930, 0.9903, 0.9913, 0.9883, 0.9863, 0.9889, 0.9871, 0.9887, 0.9869, 0.9829, 0.9882, 0.9861, 0.9854, 0.9897, 0.9875, 0.9837, 0.9878, 0.9873, 0.9839, 0.9837, 0.9863, 0.9847, 0.9835, 0.9878, 0.9867, 0.9847, 0.9866, 0.9911, 0.9843, 0.9909, 0.9886, 0.9888, 0.9862, 0.9902, 0.9948, 0.9962, 0.9902, 0.9947, 0.9963, 0.9931, 0.9955, 0.9974, 0.9939, 0.9989, 0.9981, 0.9942, 0.9986, 1.0004, 0.9999, 0.9986, 1.0049, 1.0037, 1.0045, 1.0031, 1.0096, 1.0057, 1.0065, 1.0100, 1.0103, 1.0058, 1.0097, 1.0127, 1.0115, 1.0131, 1.0137, 1.0153, 1.0086, 1.0175, 1.0148, 1.0140, 1.0166, 1.0167, 1.0176, 1.0161, 1.0177, 1.0153, 1.0155, 1.0135, 1.0182, 1.0207, 1.0164, 1.0149, 1.0172, 1.0154, 1.0169, 1.0134, 1.0112, 1.0110, 1.0125, 1.0123, 1.0127, 1.0117, 1.0109, 1.0091, 1.0048, 1.0100, 1.0048, 1.0046, 1.0020, 1.0059, 1.0034, 1.0016, 1.0019, 0.9993, 0.9990, 0.9998, 0.9993, 0.9965, 0.9962, 0.9952, 0.9933, 0.9935, 0.9941, 0.9939, 0.9908, 0.9945, 0.9918, 0.9916, 0.9926, 0.9916, 0.9883, 0.9885, 0.9885, 0.9931, 0.9887, 0.9883, 0.9935, 0.9903, 0.9920, 0.9907, 0.9871, 0.9875, 0.9897, 0.9951, 0.9924, 0.9901, 0.9939, 0.9898, 0.9903, 0.9946, 0.9913, 0.9933, 0.9920, 0.9943, 0.9935, 0.9950, 0.9987, 0.9934, 0.9986, 1.0034, 1.0004, 0.9981, 0.9985, 0.9994, 1.0019, 1.0009, 1.0017, 0.9999, 1.0028, 1.0000, 1.0061, 1.0045, 1.0036, 1.0069, 1.0044, 0.9991, 1.0030, 1.0055};
    // float compensatieAct[600] = {1.0098, 1.0094, 1.0118, 1.0102, 1.0084, 1.0082, 1.0080, 1.0080, 1.0082, 1.0096, 1.0092, 1.0087, 1.0069, 1.0074, 1.0074, 1.0070, 1.0074, 1.0060, 1.0063, 1.0047, 1.0035, 1.0025, 1.0032, 1.0045, 1.0025, 1.0000, 0.9993, 0.9996, 0.9978, 0.9984, 0.9981, 0.9967, 0.9951, 0.9939, 0.9942, 0.9929, 0.9917, 0.9924, 0.9907, 0.9894, 0.9891, 0.9883, 0.9873, 0.9870, 0.9870, 0.9859, 0.9845, 0.9839, 0.9837, 0.9840, 0.9849, 0.9836, 0.9843, 0.9819, 0.9817, 0.9828, 0.9817, 0.9834, 0.9842, 0.9823, 0.9823, 0.9827, 0.9841, 0.9839, 0.9853, 0.9853, 0.9856, 0.9863, 0.9867, 0.9877, 0.9887, 0.9901, 0.9904, 0.9910, 0.9908, 0.9921, 0.9938, 0.9946, 0.9974, 0.9982, 0.9962, 0.9985, 0.9987, 1.0013, 1.0024, 1.0041, 1.0051, 1.0050, 1.0053, 1.0066, 1.0082, 1.0097, 1.0125, 1.0125, 1.0115, 1.0127, 1.0144, 1.0152, 1.0161, 1.0171, 1.0176, 1.0176, 1.0176, 1.0182, 1.0185, 1.0198, 1.0198, 1.0210, 1.0207, 1.0205, 1.0219, 1.0216, 1.0227, 1.0220, 1.0228, 1.0217, 1.0213, 1.0205, 1.0206, 1.0211, 1.0188, 1.0178, 1.0165, 1.0157, 1.0163, 1.0157, 1.0147, 1.0135, 1.0113, 1.0096, 1.0090, 1.0086, 1.0086, 1.0073, 1.0050, 1.0043, 1.0033, 1.0015, 1.0002, 1.0006, 1.0000, 0.9986, 0.9967, 0.9963, 0.9951, 0.9955, 0.9940, 0.9945, 0.9940, 0.9913, 0.9911, 0.9914, 0.9912, 0.9907, 0.9907, 0.9883, 0.9885, 0.9884, 0.9882, 0.9881, 0.9889, 0.9883, 0.9880, 0.9873, 0.9878, 0.9879, 0.9882, 0.9896, 0.9884, 0.9886, 0.9893, 0.9900, 0.9897, 0.9918, 0.9913, 0.9919, 0.9930, 0.9924, 0.9933, 0.9949, 0.9955, 0.9976, 0.9955, 0.9968, 0.9972, 0.9991, 0.9998, 1.0008, 1.0016, 1.0016, 1.0017, 1.0029, 1.0037, 1.0051, 1.0057, 1.0067, 1.0057, 1.0061, 1.0064, 1.0082, 1.0075, 1.0089, 1.0098, 1.0090, 1.0103, 1.0110, 1.0117, 1.0129, 1.0107, 1.0104, 1.0102, 1.0085, 1.0093, 1.0099, 1.0094, 1.0109, 1.0098, 1.0084, 1.0074, 1.0078, 1.0069, 1.0073, 1.0069, 1.0071, 1.0041, 1.0043, 1.0041, 1.0034, 1.0033, 1.0028, 1.0005, 1.0000, 0.9988, 0.9989, 0.9983, 0.9975, 0.9967, 0.9959, 0.9938, 0.9938, 0.9925, 0.9927, 0.9924, 0.9919, 0.9903, 0.9892, 0.9885, 0.9882, 0.9889, 0.9873, 0.9872, 0.9853, 0.9851, 0.9856, 0.9852, 0.9856, 0.9849, 0.9854, 0.9849, 0.9845, 0.9841, 0.9855, 0.9858, 0.9861, 0.9849, 0.9845, 0.9859, 0.9862, 0.9864, 0.9876, 0.9885, 0.9883, 0.9892, 0.9889, 0.9897, 0.9908, 0.9924, 0.9931, 0.9937, 0.9923, 0.9951, 0.9953, 0.9962, 0.9987, 0.9991, 0.9994, 0.9998, 1.0012, 1.0018, 1.0040, 1.0051, 1.0063, 1.0053, 1.0063, 1.0082, 1.0079, 1.0100, 1.0118, 1.0112, 1.0112, 1.0121, 1.0130, 1.0139, 1.0153, 1.0164, 1.0160, 1.0161, 1.0162, 1.0157, 1.0179, 1.0185, 1.0178, 1.0184, 1.0186, 1.0187, 1.0188, 1.0199, 1.0208, 1.0203, 1.0194, 1.0183, 1.0185, 1.0190, 1.0195, 1.0184, 1.0164, 1.0154, 1.0146, 1.0138, 1.0126, 1.0135, 1.0120, 1.0100, 1.0094, 1.0084, 1.0056, 1.0066, 1.0067, 1.0046, 1.0037, 1.0017, 1.0012, 0.9995, 1.0001, 0.9990, 0.9975, 0.9969, 0.9944, 0.9946, 0.9938, 0.9939, 0.9930, 0.9916, 0.9908, 0.9896, 0.9889, 0.9884, 0.9897, 0.9898, 0.9880, 0.9863, 0.9864, 0.9875, 0.9855, 0.9874, 0.9873, 0.9865, 0.9856, 0.9858, 0.9852, 0.9856, 0.9875, 0.9871, 0.9871, 0.9873, 0.9873, 0.9876, 0.9871, 0.9894, 0.9893, 0.9901, 0.9897, 0.9907, 0.9909, 0.9919, 0.9944, 0.9933, 0.9946, 0.9948, 0.9948, 0.9971, 0.9969, 0.9991, 0.9985, 0.9993, 0.9990, 1.0010, 1.0005, 1.0032, 1.0046, 1.0047, 1.0037, 1.0044, 1.0058, 1.0047, 1.0075, 1.0080, 1.0076, 1.0073, 1.0092};
    float compensatieAct[600] = {1.0093, 1.0082, 1.0088, 1.0099, 1.0109, 1.0101, 1.0104, 1.0102, 1.0088, 1.0088, 1.0096, 1.0093, 1.0098, 1.0096, 1.0090, 1.0074, 1.0075, 1.0074, 1.0073, 1.0064, 1.0059, 1.0072, 1.0068, 1.0063, 1.0047, 1.0033, 1.0031, 1.0035, 1.0030, 1.0034, 1.0023, 1.0004, 1.0007, 0.9999, 0.9977, 0.9977, 0.9985, 0.9970, 0.9976, 0.9969, 0.9955, 0.9941, 0.9929, 0.9925, 0.9915, 0.9910, 0.9914, 0.9911, 0.9899, 0.9891, 0.9878, 0.9871, 0.9876, 0.9859, 0.9867, 0.9865, 0.9861, 0.9848, 0.9850, 0.9839, 0.9831, 0.9832, 0.9820, 0.9825, 0.9828, 0.9828, 0.9819, 0.9826, 0.9810, 0.9804, 0.9815, 0.9817, 0.9828, 0.9826, 0.9824, 0.9823, 0.9829, 0.9818, 0.9830, 0.9834, 0.9838, 0.9845, 0.9855, 0.9851, 0.9855, 0.9865, 0.9855, 0.9867, 0.9875, 0.9888, 0.9897, 0.9905, 0.9906, 0.9909, 0.9918, 0.9924, 0.9935, 0.9948, 0.9961, 0.9969, 0.9974, 0.9971, 0.9977, 0.9989, 1.0006, 1.0006, 1.0022, 1.0039, 1.0041, 1.0052, 1.0047, 1.0060, 1.0060, 1.0069, 1.0083, 1.0096, 1.0109, 1.0108, 1.0111, 1.0123, 1.0122, 1.0130, 1.0138, 1.0158, 1.0152, 1.0163, 1.0160, 1.0167, 1.0164, 1.0168, 1.0176, 1.0188, 1.0194, 1.0200, 1.0198, 1.0188, 1.0197, 1.0195, 1.0197, 1.0209, 1.0216, 1.0215, 1.0211, 1.0209, 1.0211, 1.0184, 1.0197, 1.0193, 1.0190, 1.0193, 1.0181, 1.0173, 1.0165, 1.0157, 1.0151, 1.0148, 1.0145, 1.0142, 1.0136, 1.0121, 1.0110, 1.0098, 1.0089, 1.0085, 1.0069, 1.0074, 1.0070, 1.0051, 1.0047, 1.0027, 1.0012, 1.0005, 1.0015, 1.0003, 1.0001, 0.9997, 0.9983, 0.9971, 0.9964, 0.9962, 0.9946, 0.9942, 0.9941, 0.9938, 0.9933, 0.9932, 0.9914, 0.9901, 0.9898, 0.9895, 0.9908, 0.9900, 0.9902, 0.9892, 0.9881, 0.9886, 0.9875, 0.9882, 0.9868, 0.9881, 0.9880, 0.9886, 0.9878, 0.9868, 0.9866, 0.9870, 0.9877, 0.9877, 0.9883, 0.9890, 0.9885, 0.9900, 0.9889, 0.9881, 0.9894, 0.9901, 0.9915, 0.9912, 0.9912, 0.9919, 0.9922, 0.9913, 0.9927, 0.9929, 0.9940, 0.9966, 0.9966, 0.9964, 0.9965, 0.9975, 0.9971, 0.9971, 0.9983, 1.0001, 1.0007, 1.0013, 1.0013, 1.0018, 1.0024, 1.0026, 1.0022, 1.0036, 1.0059, 1.0057, 1.0068, 1.0058, 1.0063, 1.0059, 1.0054, 1.0073, 1.0084, 1.0079, 1.0092, 1.0079, 1.0081, 1.0091, 1.0100, 1.0100, 1.0106, 1.0124, 1.0110, 1.0111, 1.0116, 1.0108, 1.0098, 1.0098, 1.0100, 1.0110, 1.0119, 1.0103, 1.0100, 1.0084, 1.0086, 1.0080, 1.0073, 1.0063, 1.0080, 1.0076, 1.0082, 1.0065, 1.0050, 1.0041, 1.0045, 1.0044, 1.0041, 1.0042, 1.0029, 1.0017, 1.0018, 1.0008, 0.9990, 0.9982, 0.9987, 0.9988, 0.9975, 0.9977, 0.9964, 0.9950, 0.9947, 0.9939, 0.9934, 0.9943, 0.9937, 0.9927, 0.9921, 0.9910, 0.9908, 0.9891, 0.9898, 0.9893, 0.9893, 0.9888, 0.9890, 0.9874, 0.9873, 0.9861, 0.9858, 0.9863, 0.9869, 0.9874, 0.9856, 0.9858, 0.9861, 0.9853, 0.9844, 0.9845, 0.9850, 0.9859, 0.9870, 0.9862, 0.9865, 0.9866, 0.9860, 0.9861, 0.9866, 0.9869, 0.9887, 0.9893, 0.9894, 0.9893, 0.9893, 0.9893, 0.9892, 0.9904, 0.9922, 0.9938, 0.9934, 0.9948, 0.9946, 0.9946, 0.9947, 0.9960, 0.9968, 0.9985, 0.9998, 1.0001, 0.9995, 0.9994, 1.0009, 1.0018, 1.0035, 1.0048, 1.0053, 1.0063, 1.0065, 1.0070, 1.0078, 1.0070, 1.0082, 1.0103, 1.0117, 1.0126, 1.0123, 1.0126, 1.0128, 1.0135, 1.0133, 1.0136, 1.0159, 1.0173, 1.0161, 1.0170, 1.0168, 1.0168, 1.0170, 1.0182, 1.0184, 1.0208, 1.0198, 1.0192, 1.0204, 1.0200, 1.0193, 1.0194, 1.0203, 1.0207, 1.0217, 1.0210, 1.0206, 1.0207, 1.0196, 1.0197, 1.0189, 1.0191, 1.0198, 1.0187, 1.0181, 1.0173, 1.0160, 1.0145, 1.0144, 1.0143, 1.0152, 1.0145, 1.0130, 1.0114, 1.0109, 1.0092, 1.0084, 1.0068, 1.0070, 1.0080, 1.0074, 1.0047, 1.0044, 1.0035, 1.0013, 1.0004, 1.0000, 1.0007, 1.0007, 0.9992, 0.9973, 0.9971, 0.9965, 0.9957, 0.9952, 0.9940, 0.9946, 0.9940, 0.9937, 0.9932, 0.9916, 0.9910, 0.9894, 0.9893, 0.9892, 0.9905, 0.9903, 0.9875, 0.9880, 0.9879, 0.9876, 0.9871, 0.9865, 0.9875, 0.9874, 0.9875, 0.9868, 0.9864, 0.9856, 0.9863, 0.9869, 0.9866, 0.9866, 0.9887, 0.9879, 0.9880, 0.9880, 0.9868, 0.9875, 0.9878, 0.9893, 0.9898, 0.9897, 0.9901, 0.9894, 0.9899, 0.9908, 0.9913, 0.9916, 0.9929, 0.9947, 0.9938, 0.9944, 0.9948, 0.9943, 0.9955, 0.9960, 0.9968, 0.9983, 0.9996, 0.9985, 1.0005, 0.9999, 1.0007, 1.0012, 1.0013, 1.0026, 1.0046, 1.0046, 1.0041, 1.0037, 1.0041, 1.0047, 1.0057, 1.0064, 1.0064, 1.0075, 1.0070, 1.0064, 1.0078};
    int compensatiePerSample = 8;




    COMPVAART(int samps, int p, float ppr){
      sampleNum = samps;
      pin = p;
      pulsenPerRev = ppr;
      for(int i = 0; i < sampleNum; i++){
        samples[i] = sampleMax;        
      }
    }






    void interrupt(){
      tijd = micros();

      Aoud = Anu;     
      Anu = digitalRead(plateauA);

      if(Anu && !Aoud){
        dir = 1;
        if(digitalRead(plateauB)){
          dir = -1;
        }
      }
      else{
        return;
      }   
      
      
      interval = tijd - vaartInterval;
      vaartInterval = tijd;
      
      if(interval > sampleMax){interval = sampleMax;}
      

      shiftSamples(interval * dir);
      // shiftSamples(interval);

      // teller = limieteerI(teller + dir,   0,   pulsenPerRev);  
      teller += dir;
      if(teller >= pulsenPerRev){teller = 0;}
      if(teller < 0){teller = pulsenPerRev - 1;}
    }



    



    void printSamples(){
      // Serial.print("s: ");
      // for(byte i = 0;   i < sampleNum;   i++){
      //   Serial.print(samples[i]);
      //   Serial.print(", ");
      // }
      // Serial.println();
      Serial.print("c: ");
      for(int i = 0;   i < pulsenPerRev / compensatiePerSample;   i++){
        Serial.print(compensatie[i], 4);
        Serial.print(", ");
      }
      Serial.println();
    }


    float getVaart(){
      gemiddelde = gemiddeldeInterval();
      vaart =  huidigeVaart();
      
      float b = targetRpm / vaart; //compenstaie berekenen
      compensatie[teller / compensatiePerSample]    +=    (b - compensatie[teller / compensatiePerSample]) / 12;
      
      return vaart;//niet compensenre

      // return vaart * compensatieAct[teller / compensatiePerSample]; // compenseren
    }    




    void shiftSamples(int samp){
      samples[ sampleTeller++ % sampleNum ] = samp;   
    }



    double gemiddeldeInterval(){
      double totaal = 0;
      
      for(byte i = 0;   i < sampleNum;   i++){
        totaal += samples[i];
      }
      return totaal / sampleNum;      
    }



    float huidigeVaart(){//                                                           RPM BEREKENEN
      // return (1000000 / gemiddeldeInterval())/4;  //  return totaal
      // return (((1000000.0 / gemiddelde)*60) / pulsenPerRev;  //  return totaal
      return (((1000000.0 * 60) / gemiddelde)) / pulsenPerRev;  //  return totaal

    }



};